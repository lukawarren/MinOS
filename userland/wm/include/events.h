#pragma once
#ifndef EVENTS_H
#define EVENTS_H

#include <minlib.h>

// Each message has the ID first, then the data, so that leaves
// 27 bytes per event for data

struct eWindowCreate
{
    uint32_t width;
    uint32_t height;
    char sName[19];
    
    eWindowCreate(uint32_t _width, uint32_t _height, const char* name)
    {
        width = _width;
        height = _height;
        strcpy(sName, name);
    }
} __attribute__((packed));

struct sWindowManagerEvent
{
    uint8_t id;
    eWindowCreate data;
} __attribute__((packed));

enum Events
{
    WINDOW_CREATE = 0
};

#endif
