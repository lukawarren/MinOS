#include "minlib.h"

void* operator new(size_t size)
{
    void* p = malloc(size);
    return p;
}

void* operator new[](size_t size)
{
    void* p = malloc(size);
    return p;
}

void operator delete(void * p)
{
    free(p);
}

extern "C" void __cxa_pure_virtual()
{
    printf("__cxa_pure_virtual called!\n");
}
