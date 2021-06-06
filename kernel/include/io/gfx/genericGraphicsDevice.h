#pragma once
#ifndef GENERIC_GRAPHICS_DEVICE
#define GENERIC_GRAPHICS_DEVICE

#include <stdint.h>
#include <stddef.h>

#include "io/gfx/graphicsDevice.h"
#include "multiboot.h"

namespace Framebuffer
{
    class GenericGraphicsDevice : public GraphicsDevice
    {
    public:
        GenericGraphicsDevice(const multiboot_info_t* pMultiboot);
        ~GenericGraphicsDevice() {}
    };
}

#endif