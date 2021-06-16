#pragma once
#ifndef GRPAHICS_H
#define GRAPHICS_H

#include <stdint.h>
#include <stddef.h>

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "window.h"

namespace Graphics
{
    void Init();
    void DrawRegion(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height);
    void Terminate();

    void WriteRow(const uint32_t y, const uint32_t* pData, size_t length, const size_t offset);

    extern uint32_t screenWidth;
    extern uint32_t screenHeight;
}

#endif
