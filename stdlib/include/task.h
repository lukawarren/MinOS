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

#endif