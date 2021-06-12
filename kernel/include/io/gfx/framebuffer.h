#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"
#include "io/gfx/graphicsDevice.h"

namespace Framebuffer
{
    void Init(const multiboot_info_t* pMultiboot);
    uint32_t GetColour(const uint8_t r, const uint8_t g, const uint8_t b);

    extern GraphicsDevice* graphicsDevice;
}

#endif