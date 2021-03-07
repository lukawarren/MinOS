#include "interrupts/syscall.h"

int main();

int main()
{
    while(1) {}
    sysexit();
    return 0;
}