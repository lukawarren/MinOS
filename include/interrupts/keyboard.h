#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stddef.h>
#include <stdint.h>

#include "cli.h"

class Keyboard
{
public:
    Keyboard(CLI* _cli);
    ~Keyboard();

    void OnKeyUpdate(uint8_t scancode);

    uint8_t buffer[256];

private:
    CLI* cli;
    bool shift;
};

#endif