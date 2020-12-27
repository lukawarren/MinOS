#include "interrupts/syscall.h"
#include "stdlib.h"

#include "graphics.h"
#include "bmp.h"
#include "events.h"
#include "window.h"

int main();

static Graphics graphics;

Window* pWindows = nullptr; // Linked list
Window* pCurrentWindow = nullptr;

int main()
{
    subscribeToStdout(true);
    subscribeToSysexit(true);
    printf("Starting window manager...\n");

    graphics.Init(getFramebufferWidth(), getFramebufferHeight(), getFramebufferAddr(), getFramebufferWidth() * sizeof(uint32_t));
    graphics.DrawBackground();

    uint32_t pagesProcess = loadProgram("uptime.bin");

    TaskEvent pagesEvent = {};
    pagesEvent.data[0] = 0xbe;
    pushEvent(pagesProcess, &pagesEvent);

    while (1)
    {
        // Get events
        TaskEvent* event = getNextEvent();
        while (event != NULL)
        {
            if (event->id == EVENT_QUEUE_PRINTF)
            {
                //printf((const char*)event->data);
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
                        WindowCreateMessage* message = (WindowCreateMessage*)(event->data);
                        window->x = message->x; window->width = message->width;
                        window->y = message->y; window->height = message->height;
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

            event = getNextEvent();
        }

        // Draw
        graphics.DrawBackground();
        Window* window = pWindows;
        while (window != nullptr)
        {
            graphics.DrawWindow(window->sName, window->x, window->y, window->width, window->height);
            window = (Window*) window->pNextWindow;
        }
        graphics.SwapBuffers();
    }

    sysexit();
    return 0;
}