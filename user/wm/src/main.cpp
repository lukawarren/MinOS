#include "interrupts/syscall.h"
#include "stdlib.h"

#include "graphics.h"
#include "bmp.h"

int main();

static Graphics graphics;

int main()
{
    printf("Starting window manager...", true);

    graphics.Init(getFramebufferWidth(), getFramebufferHeight(), getFramebufferAddr(), getFramebufferWidth() * sizeof(uint32_t));
    //graphics.DrawBackground();
    //graphics.DrawWindow("Terminal", 50, 50, 640 - 50*2, 480-50*2);

    // Get background file
    FileHandle file = fileOpen("background.bmp");
    void* data = malloc(getFileSize(file));
    fileRead(file, data, 0);

    auto bitmap = ParseBitmap((uint32_t)data);
    if (!bitmap.error) graphics.Blit((void*)bitmap.address);
    else printf("Error!", false);
    free((void*)bitmap.address, bitmap.size);

    free(data, getFileSize(file));
    fileClose(file);

    while(1) asm("nop");
    sysexit();
    return 0;
}