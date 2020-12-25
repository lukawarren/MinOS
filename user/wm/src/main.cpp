#include "interrupts/syscall.h"
#include "stdlib.h"

#include "graphics.h"
#include "bmp.h"
#include "events.h"

int main();

static Graphics graphics;

struct Window
{
    char sName[16];
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;

    Window() {}
    Window(const char* name, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height)
    {
        strncpy(sName, name, 16);
        x = _x;         y = _y;
        width = _width; height = _height;
    }
} __attribute__((packed));

Window windows[60];
uint32_t nWindows = 0;

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

    int bob = 0;
    while (1)
    {
        // Get events
        TaskEvent* event = getNextEvent();
        while (event != NULL)
        {
            if (event->id == EVENT_QUEUE_PRINTF)
            {
                printf((const char*)event->data);
            }

            else if (event->id == CREATE_WINDOW_EVENT)
            {
                windows[nWindows] = Window((const char*)event->data, 50, 50, 640-50*2, 480-50*2);
                nWindows++;
            }

            event = getNextEvent();
        }

        // Draw
        graphics.DrawBackground();
        for (unsigned int i = 0; i < nWindows; i++)
            graphics.DrawWindow(windows[i].sName, windows[i].x+bob, windows[i].y, windows[i].width, windows[i].height);
        graphics.SwapBuffers();

        bob++;
    }

    sysexit();
    return 0;
}