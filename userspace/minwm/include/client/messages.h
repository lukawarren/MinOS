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
message(CREATE_WINOW_MESSAGE, CreateWindowMessage, char title[32]; uint32_t width; uint32_t height; uint32_t* framebuffer;);
#ifdef __cplusplus
static_assert(sizeof(CreateWindowMessage) <= 128);
#endif

#define SWITCH_WINOW_MESSAGE 2
message(SWITCH_WINOW_MESSAGE, SwitchWindowMessage,);
#ifdef __cplusplus
static_assert(sizeof(SwitchWindowMessage) <= 128);
#endif

#define SET_WINDOW_TITLE_MESSAGE 3
message(SET_WINDOW_TITLE_MESSAGE, SetWindowTitleMessage, char title[32];);
#ifdef __cplusplus
static_assert(sizeof(SetWindowTitleMessage) <= 128);
#endif
