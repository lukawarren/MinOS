#pragma once
#include <stdint.h>
#include <unistd.h>

#define message(n, x, data)\
struct x\
{\
pid_t pid;\
int id;\
data\
} __attribute__((packed));\

#define CREATE_WINOW_MESSAGE 1
message(CREATE_WINOW_MESSAGE, CreateWindowMessage, char title[64]; uint32_t width; uint32_t height;);
#ifdef __cplusplus
static_assert(sizeof(CreateWindowMessage) <= 128);
#endif