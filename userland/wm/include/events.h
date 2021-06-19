#pragma once
#ifndef EVENTS_H
#define EVENTS_H

#include <minlib.h>

// Each message has the ID first, then the data, so that leaves
// 27 bytes per event for data

enum Events
{
    WINDOW_CREATE = 0,
    PANEL_CREATE = 1
};

struct sWindowManagerEvent
{
    uint8_t id;
    uint8_t data[27];
    
    sWindowManagerEvent(const uint8_t _id, const void* _data)
    {
        id = _id;
        memcpy(data, _data, sizeof(data));
    }
} __attribute__((packed));

struct eWindowCreate
{
    uint32_t width;
    uint32_t height;
    char sName[19];
    
    eWindowCreate(const uint32_t _width, const  uint32_t _height, const char* name) : width(_width), height(_height)
    {
        strcpy(sName, name);
        Event<sWindowManagerEvent>({WINDOW_CREATE, this}, 1);
    }
} __attribute__((packed));

struct ePanelCreate
{
    uint32_t width;
    uint32_t height;
    uint32_t x;
    uint32_t y;
    uint32_t colour;

    ePanelCreate(const uint32_t _width, const uint32_t _height, const uint32_t _x, const uint32_t _y, const uint32_t _colour) :
        width(_width), height(_height), x(_x), y(_y), colour(_colour)
    {
        Event<sWindowManagerEvent>({PANEL_CREATE, this}, 1);
    }
} __attribute__((packed));

#endif
