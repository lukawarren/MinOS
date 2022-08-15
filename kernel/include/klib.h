#pragma once

typedef unsigned int size_t;
typedef long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef int int32_t;

size_t strlen(const char* string);
char* strncpy(char* dest, const char* src, size_t maxLength);
void strcpy(char* dest, const char* source);
size_t strcmp(const char* x, const char* y);
size_t pow(const size_t number, const size_t power);
void memset(void* b, int c, size_t len);
void memcpy(void *dest, void *src, size_t n);

// To keep intellisense happy
#ifndef SOURCE_PATH_SIZE
    #define SOURCE_PATH_SIZE 0
#endif

#define assert(expr) \
    if (!(expr)) \
        error((char const*)__FILE__ + SOURCE_PATH_SIZE, __LINE__, #expr)

#define println(message) \
    _println((char const*)__FILE__ + SOURCE_PATH_SIZE, message)

void error(const char* file, unsigned int line, const char* expression);
void _println(const char* file, const char* message);