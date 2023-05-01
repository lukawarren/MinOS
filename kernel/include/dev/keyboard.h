#include "klib.h"

namespace keyboard
{
    constexpr size_t n_scancodes = 128;
    extern uint8_t scancodes[n_scancodes];
    void on_scancode(const uint8_t scancode);
}