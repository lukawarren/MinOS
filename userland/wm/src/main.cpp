#include <stdint.h>
#include <stddef.h>

#include "graphics.h"

struct Mouse
{
   int x;
   int y;
};

Mouse mouse;

void * operator new(size_t size)
{
   void* p = malloc(size);
   return p;
}

void operator delete(void * p)
{
   free(p);
}

extern "C"
{
    extern void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
}

int main();


int main()
{
    Graphics::Init();

    // Get mouse
    FILE* mouseFile = fopen("/dev/mouse", "w+");

    // Get mouse size
    struct stat mouseStat;
    fstat(mouseFile->_file, &mouseStat);
    const uint32_t mouseSize = mouseStat.st_mode;

    int* pMouse = (int*) mmap(NULL, mouseSize, PROT_WRITE | PROT_READ, MAP_SHARED, mouseFile->_file, 0);

    while (0)
    {   
        // "Clean" last position
        Graphics::DrawRegion(mouse.x - 5, mouse.y - 5, 10, 10);

        mouse.x += pMouse[0];
        mouse.y += -pMouse[1];
        pMouse[0] = 0;
        pMouse[1] = 0;
        if (mouse.x > WIDTH-1) mouse.x = WIDTH-1;
        if (mouse.y > HEIGHT-1) mouse.y = HEIGHT-1;
        if (mouse.x < 0) mouse.x = 0;
        if (mouse.y < 0) mouse.y = 0;

        for (int x = -5; x < 5; ++x)
            for (int y = -5; y < 5; ++y)
                if (x > 0 && y > 0)
                    Graphics::WritePixel(mouse.x + x, mouse.y + y, 0xffffffff);
    }

    fclose(mouseFile);
    Graphics::Terminate();
    return 0;
}