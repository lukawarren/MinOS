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

            else if (event->id == CREATE_WINDOW_EVENT)
            {
                // Enforce maximum of one window per process
                Window* window = pWindows;
                bool deny = false;
                while (window != nullptr)
                {
                    if (window->processID == event->source) deny = true;
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