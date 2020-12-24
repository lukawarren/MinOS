#include "interrupts/syscall.h"
#include "stdlib.h"

int main()
{
    uint32_t pages = nPages();
    uint32_t tasks = nTasks();
    
    printn(pages, false);
    printf(" pages used for ", false);
    printn(tasks, false);
    printf(" tasks", true);
    
    //while(1) asm("nop");
    sysexit();
    return 0;
}