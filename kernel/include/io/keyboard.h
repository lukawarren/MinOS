#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stddef.h>

#include "io/ps2.h"

namespace Keyboard
{
    void Init();
    void OnInterrupt();
}

#endif