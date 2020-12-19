#include "interrupts/syscall.h"
#include "stdlib.h"

int main()
{
    printf("Hello world!");

    sysexit();
    return 0;
}