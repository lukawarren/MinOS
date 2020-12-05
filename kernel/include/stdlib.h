#pragma once
#ifndef STDLIB_H
#define STDLIB_H

#include <stdint.h>
#include <stddef.h>

size_t  strlen(const char* string);

size_t  pow(const size_t number, const size_t power);

size_t  strcmp(const char *X, const char *Y);

char*   strncpy(char* dest, const char* src, size_t maxLength);

void    memset(void *b, int c, int len);
void    memcpy(void *dest, void *src, size_t n);

#endif