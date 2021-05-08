#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

size_t strlen(const char* string);
size_t pow(const size_t number, const size_t power);

#ifdef __cplusplus
}
#endif

#endif