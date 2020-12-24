#include "interrupts/syscall.h"
#include "stdlib.h"

#include "graphics.h"
#include "bmp.h"
#include "events.h"

int main();

static Graphics graphics;

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
        TaskEvent* event = getNextEvent();
        while (event != NULL)
        {
            if (event->id == EVENT_QUEUE_PRINTF)
            {
                printf((const char*)event->data);
            }

            else if (event->id == CREATE_WINDOW_EVENT)
            {
                graphics.DrawWindow((const char*)event->data, 50, 50, 640 - 50*2, 480-50*2);
            }

            event = getNextEvent();
        }
    }

    sysexit();
    return 0;
}