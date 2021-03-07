#include <stdint.h>
#include <stddef.h>

#include "interrupts/syscall.h"
#include "stdlib.h"
#include "task.h"

int main()
{
    uint32_t pages = nPages();
    uint32_t tasks = nTasks();

    printn(pages, true);
    printf(" pages used for ");
    printn(tasks, false);
    printf(" tasks\n");
    
    sysexit();
    return 0;
}