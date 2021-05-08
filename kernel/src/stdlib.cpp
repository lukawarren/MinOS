#include "stdlib.h"

size_t strlen(const char* string)
{
    size_t length = 0;
    while (string[length]) length++;
    return length;
}

size_t pow(const size_t number, const size_t power)
{
    if (power == 0) return 1;
    size_t i = 1;
    for (size_t j = 0; j < power; ++j) i *= number;
    return i;
}

void memset(void* b, int c, int len)
{
    unsigned char* p = (unsigned char*) b;
    while(len > 0)
    {
        *p = (unsigned char)c;
        p++;
        len--;
    }
}

void memcpy(void *dest, void *src, size_t n)
{
    char *csrc = (char *)src; 
    char *cdest = (char *)dest; 

    for (size_t i=0; i<n; i++) 
        cdest[i] = csrc[i]; 
}
