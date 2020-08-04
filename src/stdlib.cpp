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