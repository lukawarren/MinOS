#pragma once
#ifndef TASK_H
#define TASK_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
    struct TaskEvent // 32 bytes in total
    {
        uint32_t source;    // 4 bytes
        uint32_t id;        // 4 bytes
        uint8_t data[16];   // 16 bytes
    } __attribute__((packed));
}
#else
typedef struct taskEvent_t // 32 bytes in total
{
    uint32_t source;    // 4 bytes
    uint32_t id;        // 4 bytes
    uint32_t data[16];   // 16 bytes
} __attribute__((packed)) TaskEvent;
#endif

#define EVENT_QUEUE_PRINTF 0xdeadbeef

#endif