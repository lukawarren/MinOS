#include "interrupts/syscall.h"

int main()
{
    while(1) 
    {
        printf("Hello world!");
        for (int i = 0; i < 0xF00000; ++i) asm("nop");
    }


    return 0;
}