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

uint8_t* keyBuffer = nullptr;
uint8_t* keyBufferOld = nullptr;

int main()
{
    subscribeToStdout(true);
    subscribeToSysexit(true);
    subscribeToKeyboard(true);
    printf("Starting window manager...\n");

    graphics.Init(getFramebufferWidth(), getFramebufferHeight(), getFramebufferAddr(), getFramebufferWidth() * sizeof(uint32_t));

    keyBuffer =     (uint8_t*)getKeyBufferAddr();
    keyBufferOld =  (uint8_t*)malloc(256);

    loadProgram("uptime.bin");
    //loadProgram("terminal.bin");

    while (1)
    {
        // Get events
        TaskEvent* event = getNextEvent();
        while (event != nullptr)
        {
            if (event->id == EVENT_QUEUE_PRINTF)
            {
                printf((const char*)event->data);
            }

            else if (event->id == EVENT_QUEUE_KEY_PRESS)
            {
                // Special keys
                if (event->data[1])
                {
                    // Tab - cycle through windows
                    if (event->data[0] == '\t' && pCurrentWindow != nullptr)
                    {
                        if (pCurrentWindow->pNextWindow  != nullptr) pCurrentWindow = (Window*) pCurrentWindow->pNextWindow;
                        else pCurrentWindow = pWindows;
                    }

                    // Arrow keys + alt (optional) - move windows around
                    if (pCurrentWindow != nullptr && (event->data[0] == KEY_EVENT_UP || event->data[0] == KEY_EVENT_DOWN || 
                        event->data[0] == KEY_EVENT_LEFT || event->data[0] == KEY_EVENT_RIGHT))
                    {
                        if (!pCurrentWindow->bMoved) { pCurrentWindow->oldX = pCurrentWindow->x; pCurrentWindow->oldY = pCurrentWindow->y; }

                        uint32_t speed = 1 + keyBuffer[KEY_EVENT_ALT]*4;
                        
                        if (event->data[0] == KEY_EVENT_UP)     pCurrentWindow->y -= speed;
                        if (event->data[0] == KEY_EVENT_DOWN)   pCurrentWindow->y += speed;
                        if (event->data[0] == KEY_EVENT_LEFT)   pCurrentWindow->x -= speed;
                        if (event->data[0] == KEY_EVENT_RIGHT)  pCurrentWindow->x += speed;

                        // Confine window to reasonable bounds
                        if (pCurrentWindow->x + pCurrentWindow->width >= graphics.m_Width) pCurrentWindow->x = graphics.m_Width - pCurrentWindow->width;
                        if (pCurrentWindow->y + BAR_HEIGHT + pCurrentWindow->height >= graphics.m_Height) pCurrentWindow->y = graphics.m_Height - pCurrentWindow->height - BAR_HEIGHT;
                        if ((int32_t)pCurrentWindow->x < 0) pCurrentWindow->x = 0;
                        if ((int32_t)pCurrentWindow->y < 0) pCurrentWindow->y = 0;

                        pCurrentWindow->bMoved = true;
                    }

                    // Alt + ctrl - terminate current window
                    if (pCurrentWindow != nullptr && keyBuffer[KEY_EVENT_ALT] && keyBuffer[KEY_EVENT_CTRL])
                    {
                        kill(pCurrentWindow->processID);
                    }

                }

                // Alt + t - launch terminal
                else if (keyBuffer[KEY_EVENT_ALT] && keyBuffer['t']) 
                {
                    loadProgram("terminal.bin");
                }

                else if (pCurrentWindow != nullptr)
                {
                    // Send event to active window
                    TaskEvent keyEvent;
                    keyEvent.id = KEY_EVENT;
                    memset(&keyEvent.data[0], event->data[0], sizeof(char)); // GCC likes to optimise a bit too much otherwise
                    pushEvent(pCurrentWindow->processID, &keyEvent);
                }
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
                    // If in the middle
                    if (prevWindow != nullptr && window->pNextWindow != nullptr) prevWindow->pNextWindow = window->pNextWindow;
                    
                    // If last elemtent in the list but not first
                    else if (window->pNextWindow == nullptr && prevWindow != nullptr) prevWindow->pNextWindow = nullptr;

                    // If only element in list
                    if (pCurrentWindow == window && prevWindow == nullptr && window->pNextWindow == nullptr) pCurrentWindow = nullptr;
                    
                    // If not first (but current) element in list
                    else if (pCurrentWindow == window && prevWindow != nullptr) pCurrentWindow = prevWindow;
                    
                    // If first (and current) element in list, but not last
                    else if (pCurrentWindow == window && window->pNextWindow != nullptr) pCurrentWindow = (Window*) window->pNextWindow;
                    
                    // If beginning of list and only one window
                    if  (window == pWindows && window->pNextWindow == nullptr) pWindows = nullptr;

                    // If beginning of list but there's more than one window
                    if (window == pWindows && window->pNextWindow != nullptr) pWindows = (Window*) window->pNextWindow;

                    graphics.OnWindowDestroy(*window);

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
                        if (!window->bMoved) { window->oldX = window->x; window->oldY = window->y; }
                        window->x = message->x; window->width = message->width;
                        window->y = message->y - BAR_HEIGHT; 
                        window->height = message->height;
                        window->bMoved = true;

                        // Recreate window if there has been a resize
                        if (window->width != oldWidth || window->height != oldHeight)
                        {
                            free(window->buffer, sizeof(uint32_t)*window->width*(window->height+BAR_HEIGHT));
                            window->buffer = malloc(sizeof(uint32_t)*window->width*(window->height + BAR_HEIGHT));
                            memset(window->buffer, WINDOW_BACKGROUND_COLOUR, sizeof(uint32_t)*window->width*(window->height + BAR_HEIGHT));
                        }

                        break; 
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
                    *pCurrentWindow = Window("Untitled", message->x, message->y - BAR_HEIGHT, message->width, message->height, event->source);
                    window->buffer = malloc(sizeof(uint32_t)*window->width*(window->height + BAR_HEIGHT));
                    memset(window->buffer, WINDOW_BACKGROUND_COLOUR, sizeof(uint32_t)*window->width*(window->height+BAR_HEIGHT));
                }

                // Draw window
                if (deny) graphics.OnWindowMove(*window);
                graphics.DrawWindow(*window);
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
                        graphics.OnWindowTitleChange(*window);
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
                        // No need to translate coordinates to window space
                        WindowDrawString* message = (WindowDrawString*)event->data;
                        uint32_t stringWidth = CHAR_WIDTH*strlen(message->message);
                        uint32_t stringHeight = CHAR_HEIGHT*CHAR_SCALE;
                        uint32_t stringX = message->x;
                        uint32_t stringY = message->y;

                        // Check confines
                        bool deny = ((int32_t)stringX < 0 || (int32_t)stringY < 0 || stringX + stringWidth >= window->width || stringY + stringHeight >= window->height);

                        // Draw string
                        if (!deny) graphics.DrawString(message->message, stringX, stringY + BAR_HEIGHT, message->colour, *window);

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
                        if (!deny) graphics.DrawNumber(message->number, numberX, numberY, message->colour, message->hex, *window);

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
            if (event->id != EVENT_QUEUE_PRINTF && event->id != EVENT_QUEUE_KEY_PRESS && event->id != EVENT_QUEUE_SYSEXIT)
            {
                TaskEvent unblockEvent;
                unblockEvent.id = UNBLOCK_EVENT;
                pushEvent(event->source, &unblockEvent);
            }
            
            event = getNextEvent();
        }

        // Move windows that need to be moved
        Window* window = pWindows;
        while (window != nullptr)
        {
            if (window->bMoved)
            {
                graphics.OnWindowMove(*window);
                graphics.DrawWindow(*window);
                window->bMoved = false;
            }
            window = (Window*) window->pNextWindow;
        }

        // Draw background and window and swap buffers
        graphics.DrawFrame(pWindows);
    }

    sysexit();
    return 0;
}
