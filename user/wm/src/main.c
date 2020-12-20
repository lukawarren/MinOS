#include "interrupts/syscall.h"
#include "stdlib.h"

int main()
{
    printf("Starting window manager...", false);

    while(1) asm("nop");

    sysexit();
    return 0;
}