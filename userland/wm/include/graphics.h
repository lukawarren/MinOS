#pragma once
#ifndef GRPAHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "../../../kernel/include/multitask/mman.h"

namespace Graphics
{
    struct Window
    {
        uint32_t x;
        uint32_t y;
        uint32_t width;
        uint32_t height;
    };

    void Init();
    void DrawRegion(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
    void Terminate();

    extern "C"
    {
        extern void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset);
    }

    constexpr inline uint32_t GetColour(const uint32_t r, const uint32_t g, const uint32_t b)
    {
        const auto a = 0xff;
        return a << 24 | r << 16 | g << 8 | b;
    }

    void WritePixel(const uint32_t x, const uint32_t y, const uint32_t colour);
}

#endif