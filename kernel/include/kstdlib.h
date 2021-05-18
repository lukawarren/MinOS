#ifndef KSTDLIB_H
#define KSTDLIB_H

#include <stdint.h>
#include <stddef.h>
#include <stdout/uart.h>

size_t strlen(const char* string);
char* strncpy(char* dest, const char* src, size_t maxLength);
void strcpy(char* dest, const char* source);
size_t strcmp(const char* x, const char* y);
size_t pow(const size_t number, const size_t power);
void memset(void* b, int c, int len);
void memcpy(void *dest, void *src, size_t n);

#define assert(expr) \
    if (!(expr)) \
        error(__FILE__, __LINE__, #expr)

inline void error(const char* file, unsigned int line, const char* expression)
{
    UART::WriteString("Assertion failed in ");
    UART::WriteString(file);
    UART::WriteString(" - line ");
    UART::WriteNumber(line);
    UART::WriteString(": ");
    UART::WriteString(expression);
    UART::WriteString("\n");
}

#endif