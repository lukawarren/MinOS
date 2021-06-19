#pragma once
#ifndef EVENTS_H
#define EVENTS_H

#include <minlib.h>

// Each message has the ID first, then the data, so that leaves
// 27 bytes per event for data

enum Events
{
    WINDOW_CREATE = 0
};

struct sWindowManagerEvent
{
    uint8_t id;
    uint8_t data[27];
    
    sWindowManagerEvent(uint8_t _id, void* _data)
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
    
    eWindowCreate(uint32_t _width, uint32_t _height, const char* name)
    {
        width = _width;
        height = _height;
        strcpy(sName, name);
        
        Event<sWindowManagerEvent>({WINDOW_CREATE, this}, 1);
    }
} __attribute__((packed));

#endif
