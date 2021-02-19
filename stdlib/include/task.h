#pragma once
#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
    struct TaskEvent // 42 bytes in total
    {
        uint32_t source;    // 4 bytes
        uint32_t id;        // 4 bytes
        uint8_t data[32];   // 32 bytes
    } __attribute__((packed));
}
#else
typedef struct taskEvent_t // 42 bytes in total
{
    uint32_t source;    // 4 bytes
    uint32_t id;        // 4 bytes
    uint8_t data[32];   // 32 bytes
} __attribute__((packed)) TaskEvent;
#endif

#define EVENT_QUEUE_PRINTF 0xdeadbeef
#define EVENT_QUEUE_SYSEXIT 0xefefefe
#define EVENT_QUEUE_KEY_PRESS 0x1234321

#define KEY_EVENT_ALT   0x1
#define KEY_EVENT_DOWN  0x2
#define KEY_EVENT_UP    0x3
#define KEY_EVENT_LEFT  0x4
#define KEY_EVENT_RIGHT 0x5
#define KEY_EVENT_CTRL  0x6
#define KEY_EVENT_BACKSPACE 0x7

#endif