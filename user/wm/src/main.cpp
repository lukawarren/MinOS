#include "interrupts/syscall.h"
#include "stdlib.h"

#include "graphics.h"

int main();

static Graphics graphics;

int main()
{
    printf("Starting window manager...", false);

    graphics.Init(getFramebufferWidth(), getFramebufferHeight(), getFramebufferAddr(), getFramebufferWidth() * sizeof(uint32_t));
    graphics.DrawBackground();
    graphics.DrawWindow("Terminal", 20, 20, 640 - 20*2, 480-20*2);

    while(1) asm("nop");
    sysexit();
    return 0;
}