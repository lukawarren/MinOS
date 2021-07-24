#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stddef.h>

#include "io/ps2.h"

#define KEYBOARD_SCANCODE_KEY_RELEASE 0x80

namespace Keyboard
{
    void Init();
    void OnInterrupt();
}

#endif
