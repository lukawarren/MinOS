#include <stdint.h>
#include <stddef.h>

#include "interrupts/syscall.h"
#include "stdlib.h"
#include "task.h"
#include "events.h"
#include "colours.h"

int main()
{
    CreateWindow(0, 0, 200, 36);
    SetWindowTitle("Uptime");

    const char* message = "Total uptime (s): ";
    DrawWindowString(message, 10, 10, WINDOW_WHITE);

    CreateWindow(100, 100, 200, 36);

    unsigned int count1 = 0;
    unsigned int count2 = 0;
    while (1)
    {
        CreateWindow(count2, count2, 200, 36);
        DrawWindowNumber(getSeconds(), 10 + strlen(message)*8+8, 10, WINDOW_WHITE, false);
        
        count1++;
        if (count1 % 5000 == 0) count2++;
    }

    sysexit();
    return 0;
}