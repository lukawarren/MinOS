#include <stdint.h>
#include <stddef.h>

#include "interrupts/syscall.h"
#include "stdlib.h"
#include "task.h"
#include "events.h"

int main()
{
    CreateWindow(0, 0, 400, 400);
    SetWindowTitle("Uptime");

    sysexit();
    return 0;
}