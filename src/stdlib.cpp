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

size_t strcmp(const char *X, const char *Y)
{
	while(*X)
	{
		if (*X != *Y)
			break;

		X++; Y++;
	}
	return (*(const unsigned char*)X - *(const unsigned char*)Y) == 0;
}

char* strncpy(char* dest, const char* src, size_t maxLength)
{
    char *temp = dest;
    while(maxLength-- && (*dest++ = *src++)); // or while((*strDest++=*strSrc++) != '\0');
    return temp;
}

void memset(void* b, int c, int len)
{
    int i;
    unsigned char* p = (unsigned char*) b;
    i = 0;
    while(len > 0)
    {
        *p = c;
        p++;
        len--;
    }
}