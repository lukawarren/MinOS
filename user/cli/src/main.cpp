#include "interrupts/syscall.h"
#include "stdlib.h"

int main();

int main()
{
    subscribeToStdout(true);
    subscribeToSysexit(true);
    subscribeToKeyboard(true);
    printf("Starting window manager...\n");
    
    while(1) {}

    sysexit();
    return 0;
}
