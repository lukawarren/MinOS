#pragma once
#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>
#include <stddef.h>

#include "io/ps2.h"

#define MOUSE_BYTE_LEFT_BUTTON      (1 << 0)
#define MOUSE_BYTE_RIGHT_BUTTON     (1 << 1)
#define MOUSE_BYTE_MIDDLE_BUTTON    (1 << 2)
#define MOUSE_BYTE_X_SIGN_BIT       (1 << 4)
#define MOUSE_BYTE_Y_SIGN_BIT       (1 << 5)
#define MOUSE_BYTE_X_DID_OVERFLOW   (1 << 6)
#define MOUSE_BYTE_Y_DID_OVERFLOW   (1 << 7)

namespace Mouse
{
    void Init();
    void OnInterrupt();
}

#endif