#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define WIDTH 1024
#define HEIGHT 768

int main()
{
   printf("Hello world!\n");
  
   // Create red "buffer"
   uint32_t* data = malloc(sizeof(uint32_t) * WIDTH * HEIGHT);
   for (uint32_t y = 0; y < HEIGHT; ++y)
      for (uint32_t x = 0; x < WIDTH; ++x)
         data[y * WIDTH + x] = 0xffff0000;
   
   FILE* fp = fopen("/dev/fb", "w+");
   fputs(0, fp);
   fclose(fp);

   printf("Goodbye world!\n");
   return 0;
}