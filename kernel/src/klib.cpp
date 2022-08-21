#include "klib.h"
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
    while(maxLength-- && (*dest++ = *src++));
    return temp;
}

void strcpy(char* dest, const char* source)
{
    int i = 0;
    while ((dest[i] = source[i]) != '\0') i++;
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

void memset(void* b, int c, size_t len)
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

void __cxa_pure_virtual() { assert(false); }
void atexit() { assert(false); }

void error(const char* file, unsigned int line, const char* expression)
{
    uart::write_string("Assertion failed in ");
    uart::write_string(file);
    uart::write_string(" - line ");
    uart::write_number(line);
    uart::write_string(": ");
    uart::write_string(expression);
    uart::write_string("\n");
}

void _println(const char* file, const char* message)
{
    uart::write_string("[");
    uart::write_string(file);
    uart::write_string("] ");
    uart::write_string(message);
    uart::write_char('\n');
}

void _println(const char* file, const char* message, const char* message_two)
{
    uart::write_string("[");
    uart::write_string(file);
    uart::write_string("] ");
    uart::write_string(message);
    uart::write_string(message_two);
    uart::write_char('\n');
}

void _println(const char* file, const char* message, const int number)
{
    // TODO: support signs
    _println(file, message, (size_t)number);
}

void _println(const char* file, const char* message, const size_t number)
{
    uart::write_string("[");
    uart::write_string(file);
    uart::write_string("] ");
    uart::write_string(message);
    uart::write_number(number);
    uart::write_char('\n');
}

void _println(const char* file, const char* message, const size_t number, const char* message_two, const size_t number_two)
{
    uart::write_string("[");
    uart::write_string(file);
    uart::write_string("] ");
    uart::write_string(message);
    uart::write_number(number);
    uart::write_string(message_two);
    uart::write_number(number_two);
    uart::write_char('\n');
}
