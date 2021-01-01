#include <stdint.h>
#include <stddef.h>

#include "interrupts/syscall.h"
#include "stdlib.h"
#include "task.h"
#include "events.h"
#include "colours.h"

int main()
{
    CreateWindow(10, getFramebufferHeight()-36-10, 200, 36);
    SetWindowTitle("Uptime");

    const char* message = "Total uptime (s): ";
    DrawWindowString(message, 10, 10, WINDOW_WHITE);

    while (1)
    {
        DrawWindowNumber(getSeconds(), 10 + strlen(message)*8+8, 10, WINDOW_WHITE, false);
    }

    sysexit();
    return 0;
}