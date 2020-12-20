#include "interrupts/syscall.h"
#include "stdlib.h"

int main()
{
    uint32_t pages = (uint32_t) nPages();
    uint32_t tasks = (uint32_t) nTasks();

    printn(pages, false);
    printf(" pages used for ", false);
    printn(tasks, false);
    printf(" tasks", false);

    sysexit();
    return 0;
}