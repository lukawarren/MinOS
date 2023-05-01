#include "dev/keyboard.h"

namespace keyboard
{
    uint8_t scancodes[n_scancodes] = {};

    void on_scancode(const uint8_t scancode)
    {
        const bool released = (scancode & 0x80);
        const uint8_t key = (scancode & 0x7f);
        scancodes[key] = !released;
    }
}