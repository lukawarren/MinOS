#pragma once
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"

namespace Framebuffer
{
    struct Framebuffer
    {
        uint32_t address;
        uint32_t width;
        uint32_t height;
        uint32_t pitch;
        uint32_t size;

        void SetPixel(uint32_t x, uint32_t y, uint32_t colour)
        {
            *((uint32_t*)address + y*width + x) = colour;
        }
    };

    void Init(const multiboot_info_t* pMultiboot);
    uint32_t GetColour(const uint8_t r, const uint8_t g, const uint8_t b);

    extern Framebuffer sFramebuffer;
}

#endif