#pragma once
#include "klib.h"

namespace interrupts
{
    void load();

    constexpr size_t keyboard_buffer_size = 64;
    extern uint8_t keyboard_buffer[keyboard_buffer_size];
    extern size_t keyboard_buffer_keys;
}