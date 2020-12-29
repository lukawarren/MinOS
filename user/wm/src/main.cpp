#include "interrupts/syscall.h"
#include "stdlib.h"
#include "window.h"
#include "graphics.h"
#include "bmp.h"
#include "events.h"

int main();

static Graphics graphics;

Window* pWindows = nullptr; // Linked list
Window* pCurrentWindow = nullptr;

int main()
{
    subscribeToStdout(true);
    subscribeToSysexit(true);
    subscribeToKeyboard(true);
    printf("Starting window manager...\n");

    graphics.Init(getFramebufferWidth(), getFramebufferHeight(), getFramebufferAddr(), getFramebufferWidth() * sizeof(uint32_t));
    graphics.DrawBackground();

    loadProgram("uptime.bin");
    loadProgram("terminal.bin");

    while (1)
    {
        // Get events
        TaskEvent* event = getNextEvent();
        while (event != nullptr)
        {
            if (event->id == EVENT_QUEUE_PRINTF)
            {
                //printf((const char*)event->data);
            }

            else if (event->id == EVENT_QUEUE_KEY_PRESS)
            {
                printf("Keyboard input");
            }

            else if (event->id == EVENT_QUEUE_SYSEXIT)
            {
                // Find window (if any)
                Window* window = pWindows;
                Window* prevWindow = nullptr;
                bool found = false;
                while (window != nullptr)
                {
                    if (window->processID == event->source) 
                    {
                        found = true;
                        break;
                    }
                    prevWindow = window;
                    window = (Window*) window->pNextWindow;
                }

                // Destroy window
                if (found)
                {
                    if (prevWindow != nullptr && window->pNextWindow != nullptr) prevWindow->pNextWindow = window->pNextWindow;
                    if (pCurrentWindow == window) pCurrentWindow = nullptr;
                    if (window == pWindows) pWindows = nullptr;
                    
                    free(window->buffer, sizeof(uint32_t)*window->width*window->height);
                    free(window, sizeof(Window));
                }
            }

            else if (event->id == CREATE_WINDOW_EVENT)
            {
                // Enforce maximum of one window per process by replacing
                // window if nessecary
                Window* window = pWindows;
                bool deny = false;
                while (window != nullptr)
                {
                    if (window->processID == event->source) 
                    {
                        deny = true;

                        uint32_t oldWidth = window->width;
                        uint32_t oldHeight = window->height;

                        WindowCreateMessage* message = (WindowCreateMessage*)(event->data);
                        window->x = message->x; window->width = message->width;
                        window->y = message->y; window->height = message->height;

                        // Recreate window if there has been a resize
                        if (window->width != oldWidth || window->height != oldHeight)
                        {
                            free(window->buffer, sizeof(uint32_t)*window->width*window->height);
                            window->buffer = malloc(sizeof(uint32_t)*window->width*window->height);
                            memset(window->buffer, WINDOW_BACKGROUND_COLOUR, sizeof(uint32_t)*window->width*window->height);
                        }     
                    }
                    window = (Window*) window->pNextWindow;
                }

                if (!deny)
                {
                    // Append to linked list
                    window = (Window*) malloc(sizeof(Window));
                    if (pWindows == nullptr) pWindows = window;
                    else pCurrentWindow->pNextWindow = window;
                    pCurrentWindow = window;

                    // Set data
                    WindowCreateMessage* message = (WindowCreateMessage*)(event->data);
                    *pCurrentWindow = Window("Untitled", message->x, message->y, message->width, message->height, event->source);
                    window->buffer = malloc(sizeof(uint32_t)*window->width*window->height);
                    memset(window->buffer, WINDOW_BACKGROUND_COLOUR, sizeof(uint32_t)*window->width*window->height);
                }
            }

            else if (event->id == SET_TITLE_EVENT)
            {
                // Find window and set data
                Window* window = pWindows;
                while (window != nullptr)
                {
                    if (window->processID == event->source)
                    {
                        strncpy(window->sName, (char*)event->data, 16);
                        break;
                    }
                    window = (Window*) window->pNextWindow;
                }
            }

            else if (event->id == DRAW_STRING_EVENT)
            {
                // Find window and draw string
                Window* window = pWindows;
                while (window != nullptr)
                {
                    if (window->processID == event->source)
                    {
                        // Translate coordinates to window space
                        WindowDrawString* message = (WindowDrawString*)event->data;
                        uint32_t stringWidth = CHAR_WIDTH*strlen(message->message);
                        uint32_t stringHeight = CHAR_HEIGHT*CHAR_SCALE;
                        uint32_t stringX = message->x;
                        uint32_t stringY = message->y;

                        // Check confines
                        bool deny = ((int32_t)stringX < 0 || (int32_t)stringY < 0 || stringX + stringWidth >= window->width || stringY + stringHeight >= window->height);

                        // Draw string
                        if (!deny) graphics.DrawString(message->message, stringX, stringY, message->colour, window->buffer, window->width * sizeof(uint32_t));

                        break;
                    }
                    window = (Window*) window->pNextWindow;
                }
            }

            else if (event->id == DRAW_NUMBER_EVENT)
            {
                // Find window and draw string
                Window* window = pWindows;
                while (window != nullptr)
                {
                    if (window->processID == event->source)
                    {
                        WindowDrawNumber* message = (WindowDrawNumber*)event->data;
                        uint32_t numberWidth = CHAR_WIDTH*graphics.GetDigits(message->number, message->hex ? 16 : 10);
                        uint32_t numberHeight = CHAR_HEIGHT*CHAR_SCALE;
                        uint32_t numberX = message->x;
                        uint32_t numberY = message->y;

                        // Check confines
                        bool deny = ((int32_t)numberX < 0 || (int32_t)numberY < 0 || numberX + numberWidth >= window->width || numberY + numberHeight >= window->height);

                        // Draw string
                        if (!deny) graphics.DrawNumber(message->number, numberX, numberY, message->colour, window->buffer, window->width * sizeof(uint32_t));

                        break;
                    }
                    window = (Window*) window->pNextWindow;
                }
            }

            else
            {
                printf("Error: Unknown event!\n");
            }

            // Send acknowledgement event
            if (event->id != EVENT_QUEUE_PRINTF && event->id != EVENT_QUEUE_KEY_PRESS)
            {
                TaskEvent unblockEvent;
                unblockEvent.id = UNBLOCK_EVENT;
                pushEvent(event->source, &unblockEvent);
            }

            event = getNextEvent();
        }
        
        // Draw
        graphics.DrawBackground();
        Window* window = pWindows;
        while (window != nullptr)
        {
            graphics.DrawWindow(window->sName, window->x, window->y, window->width, window->height, window->buffer);
            window = (Window*) window->pNextWindow;
        }

        graphics.SwapBuffers();
    }

    sysexit();
    return 0;
}