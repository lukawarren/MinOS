#pragma once
#ifndef BMP_H
#define BMP_H

#include <stdint.h>
#include <stddef.h>

struct Bitmap
{
    uint32_t address;
    uint32_t size;
    uint8_t error;
};
#include "graphics.h"
Bitmap ParseBitmap(uint32_t address);

#endif