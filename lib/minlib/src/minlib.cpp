#include "minlib.h"

void* operator new(size_t size)
{
   void* p = malloc(size);
   return p;
}

void operator delete(void * p)
{
   free(p);
}
