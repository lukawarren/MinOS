#include "kstdlib.h"
#include "io/uart.h"

size_t strlen(const char* string)
{
    size_t length = 0;
    while (string[length]) length++;
    return length;
}

char* strncpy(char* dest, const char* src, size_t maxLength)
{
    char *temp = dest;
    while(maxLength-- && (*dest++ = *src++)); // or while((*strDest++=*strSrc++) != '\0');
    return temp;
}

void strcpy(char* dest, const char* source) 
{
    int i = 0;
    while ((dest[i] = source[i]) != '\0')
    {
        i++;
    } 
}

size_t strcmp(const char *x, const char *y)
{
	while(*x)
	{
		if (*x != *y)
			break;

		x++; y++;
	}
	return (*(const unsigned char*)x - *(const unsigned char*)y) == 0;
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

void error(const char* file, unsigned int line, const char* expression)
{
    UART::WriteString("Assertion failed in ");
    UART::WriteString(file);
    UART::WriteString(" - line ");
    UART::WriteNumber(line);
    UART::WriteString(": ");
    UART::WriteString(expression);
    UART::WriteString("\n");
}

void * operator new(size_t size)
{
   return (void*) Memory::kPageFrame.AllocateMemory(size, KERNEL_PAGE);
}

extern "C" void __cxa_pure_virtual()
{
    UART::WriteString("[Stdlib] __cxa_pure_virtual exception\n");
}
