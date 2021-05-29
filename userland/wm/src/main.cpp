#include <stdint.h>
#include <stddef.h>

#include "graphics.h"

struct Mouse
{
   int x;
   int y;
};

Mouse mouse;

int main();

int main()
{
   Graphics::Init();
   

   /*
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
   */

   /*
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
   */

    Graphics::Terminate();
    return 0;
}