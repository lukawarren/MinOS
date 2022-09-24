#pragma once
#include <stdint.h>
#include <unistd.h>

struct Message
{
    pid_t pid;
    uint8_t data[124];
} __attribute__((packed));
