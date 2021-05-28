#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#include "../../../kernel/include/multitask/mman.h"

#define WIDTH 1024
#define HEIGHT 768

extern "C"
{
   extern void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
}

constexpr inline uint32_t GetColour(const uint32_t r, const uint32_t g, const uint32_t b)
{
   const uint32_t a = 0xff;
   return a << 24 | r << 16 | g << 8 | b;
}

constexpr uint32_t baseWindowColour = GetColour(212, 208, 200);
constexpr uint32_t leadingEdgeColour = GetColour(128, 128, 128);
constexpr uint32_t trimmingColour = GetColour(255, 255, 255);
constexpr uint32_t barShadeLight = 187;
constexpr uint32_t barShadeDark = 129;
constexpr uint32_t weirdBarShade = GetColour(100, 100, 100);

struct Window
{
   uint32_t x;
   uint32_t y;
   uint32_t width;
   uint32_t height;
};

struct Mouse
{
   int x;
   int y;
};

Mouse mouse;

int main();

int main()
{
   
   // Open framebuffer
   FILE* framebufferFile = fopen("/dev/fb", "w+");

   // Get size
   struct stat framebufferStat;
   fstat(framebufferFile->_file, &framebufferStat);
   const uint32_t framebufferSize = framebufferStat.st_mode;

   // Map into memory
   uint32_t* pFramebuffer = (uint32_t*) mmap(NULL, framebufferSize, PROT_WRITE | PROT_READ, MAP_SHARED, framebufferFile->_file, 0);
   
   // Clear framebuffer
   for (uint32_t i = 0; i < WIDTH*HEIGHT; ++i)
         pFramebuffer[i] = 0;

   const auto WritePixel = [&](const uint32_t x, const uint32_t y, const uint32_t colour)
   {
      pFramebuffer[y * WIDTH + x] = colour;
   };

   // "Make" (it's a strong word) a window
   Window window = { 100, 100, 500, 500 };

   // Draw window background (save for leading edge)
   for (uint32_t y = 0; y < window.height; ++y)
      for (uint32_t x = 0; x < window.width; ++x)
         WritePixel(window.x + x, window.y + y, baseWindowColour);

   // Draw right ledaing edge
   for (uint32_t y = 1; y < window.height; ++y)
      WritePixel(window.x + window.width - 1, window.y + y, leadingEdgeColour);

   // Draw bottom ledaing edge
   for (uint32_t x = 1; x < window.width; ++x)
      WritePixel(window.x + x, window.y + window.height - 1, leadingEdgeColour);

   // White trim
   for (uint32_t x = 1; x < window.width - 1; ++x)
      WritePixel(window.x + x, window.y + 1, trimmingColour);
   for (uint32_t y = 1; y < window.height - 1; ++y)
      WritePixel(window.x + 1, window.y + y, trimmingColour);

   // Draw bar
   const uint32_t barWidth = window.width - 3 - 4;
   for (uint32_t x = 4; x < window.width - 2; ++x)
      for (uint32_t y = 4; y < 4+19; ++y)
      {
         // Work out gradient
         const float progress = float(x - 4) / (float)barWidth;
         const uint32_t difference = barShadeLight - barShadeDark;
         const uint32_t shade = uint32_t(progress * difference + barShadeDark);

         // Add weird lines
         const bool bIsWeirdLine = x > 40 && x < barWidth - 40 && y % 2 == 0 && y > 6 && y < 20;

         WritePixel(window.x + x, window.y + y, bIsWeirdLine ? weirdBarShade : GetColour(shade, shade, shade));
      }

   // Get mouse
   FILE* mouseFile = fopen("/dev/mouse", "w+");

   // Get mouse size
   struct stat mouseStat;
   fstat(mouseFile->_file, &mouseStat);
   const uint32_t mouseSize = mouseStat.st_mode;

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

      for (int x = -5; x < 5; ++x)
         for (int y = -5; y < 5; ++y)
            if (x > 0 && y > 0)
               WritePixel(mouse.x + x, mouse.y + y, 0xffffffff);
   }

   fclose(framebufferFile);
   fclose(mouseFile);
   return 0;
}