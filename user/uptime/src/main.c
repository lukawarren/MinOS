#include <stdint.h>
#include <stddef.h>

#include "interrupts/syscall.h"
#include "stdlib.h"
#include "task.h"
#include "events.h"

int main()
{
    CreateWindow(0, 0, 100, 100);
    SetWindowTitle("Uptime");

    sysexit();
    return 0;
}