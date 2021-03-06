#pragma once
#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>
#include <stddef.h>

#define assert(expr) \
    if (!(expr)) \
        error(__FILE__, __LINE__, #expr)

#ifdef __cplusplus
extern "C"
{
#endif

size_t  strlen(const char* string);

size_t  pow(const size_t number, const size_t power);

size_t  strcmp(const char *X, const char *Y);

char*   strncpy(char* dest, const char* src, size_t maxLength);

void    memset(void *b, int c, int len);
void    memcpy(void *dest, void *src, size_t n);

void    error(const char* file, unsigned int line, const char* expression);

struct Registers
{
    uint32_t esi;
    uint32_t edi;
    uint32_t ebp;
    uint32_t edx;
    uint32_t ecx;
    uint32_t ebx;
    uint32_t eax;
}  __attribute__((packed));

#ifdef __cplusplus
}
#endif

#endif