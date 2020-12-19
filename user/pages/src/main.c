#include "interrupts/syscall.h"
#include "stdlib.h"

int main()
{
    char bob[100] = "Hello world!";
    char jeff[100];

    strncpy(jeff, bob, 100);
    printf(jeff);
    
    printf("Hello world!");

    sysexit();
    return 0;
}