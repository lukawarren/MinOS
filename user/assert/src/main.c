#include "interrupts/syscall.h"
#include "stdlib.h"

int main();

int main()
{
    assert(2 + 2 == 5);

    sysexit();
    return 0;
}