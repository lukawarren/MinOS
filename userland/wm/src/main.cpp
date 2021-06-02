#include <stdint.h>
#include <stddef.h>

#include "graphics.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

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
    extern void* munmap(void* addr, size_t length);
}

int main();

extern Graphics::Window* window;

int main()
{
    Graphics::Init();

    // Get mouse
    FILE* mouseFile = fopen("/dev/mouse", "w+");

    // Get mouse size
    struct stat mouseStat;
    fstat(mouseFile->_file, &mouseStat);
    const uint32_t mouseSize = mouseStat.st_size;

    int* pMouse = (int*) mmap(NULL, mouseSize, PROT_WRITE | PROT_READ, MAP_SHARED, mouseFile->_file, 0);

    while (1)
    {   
        mouse.x += pMouse[0];
        mouse.y += -pMouse[1];
        pMouse[0] = 0;
        pMouse[1] = 0;
        if (mouse.x > WIDTH-1) mouse.x = WIDTH-1;
        if (mouse.y > HEIGHT-1) mouse.y = HEIGHT-1;
        if (mouse.x < 0) mouse.x = 0;
        if (mouse.y < 0) mouse.y = 0;

        uint32_t oldX = window->m_X;
        uint32_t oldY = window->m_Y;
        uint32_t oldEndX = oldX + window->m_Width;
        uint32_t oldEndY = oldY + window->m_Height;

        delete window;
        window = new Graphics::Window(600, 400, mouse.x, mouse.y, "Terminal");

        uint32_t chosenX = MIN(oldX, window->m_X);
        uint32_t chosenY = MIN(oldY, window->m_Y);
        uint32_t chosenEndX = MAX(oldEndX, window->m_Width + window->m_X);
        uint32_t chosenEndY = MAX(oldEndY, window->m_Height + window->m_Y);

        Graphics::DrawRegion(chosenX, chosenY, chosenEndX - chosenX, chosenEndY - chosenY);
    }

    fclose(mouseFile);
    Graphics::Terminate();
    return 0;
}