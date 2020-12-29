#include <stdint.h>
#include <stddef.h>

#include "interrupts/syscall.h"
#include "stdlib.h"
#include "task.h"
#include "events.h"
#include "colours.h"

int main()
{
    CreateWindow(200, 200, 200, 200);
    SetWindowTitle("Terminal");

    DrawWindowString("$ ", 10, 10, WINDOW_WHITE);
    
    while (1)
    {
        
    }

    sysexit();
    return 0;
}