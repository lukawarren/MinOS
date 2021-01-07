#ifndef WINDOW_H
#define WINDOW_H

#include <stdint.h>
#include <stddef.h>
#include "stdlib.h"

struct Window
{
    char sName[32];
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t processID;
    void* buffer;
    void* pNextWindow = nullptr;

    uint32_t oldX = 0xFFFFFFFF;
    uint32_t oldY = 0xFFFFFFFF;

    bool bMoved = false;

    Window() {}
    Window(const char* name, uint32_t _x, uint32_t _y, uint32_t _width, uint32_t _height, uint32_t id)
    {
        strncpy(sName, name, 32);
        x = _x;         y = _y;
        width = _width; height = _height;
        processID = id;
    }
} __attribute__((packed));

#endif