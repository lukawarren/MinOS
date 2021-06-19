#pragma once
#ifndef EVENTS_H
#define EVENTS_H

#include <minlib.h>

#define WINDOW_MANAGER_PID 1

// Each message has the ID first, then the data, so that leaves
// 27 bytes per event for data

enum Events
{
    WINDOW_CREATE = 0,
    WINDOW_CLOSE = 1,
    PANEL_CREATE = 2,
    EXIT = 3
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
    
    sWindowManagerEvent(const uint8_t _id) : id(_id) {}
    
} __attribute__((packed));

struct eWindowCreate
{
    uint32_t width;
    uint32_t height;
    char sName[19];
    
    eWindowCreate(const uint32_t _width, const  uint32_t _height, const char* name) : width(_width), height(_height)
    {
        strcpy(sName, name);
        Event<sWindowManagerEvent>({WINDOW_CREATE, this}, WINDOW_MANAGER_PID);
    }
} __attribute__((packed));

struct eWindowClose
{
    eWindowClose()
    {
        Event<sWindowManagerEvent>({WINDOW_CLOSE, this}, WINDOW_MANAGER_PID);
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
        Event<sWindowManagerEvent>({PANEL_CREATE, this}, WINDOW_MANAGER_PID);
    }
} __attribute__((packed));

struct eExit
{
    uint32_t exitCode;
    
    eExit(const uint32_t pid = 0, const uint32_t _exitCode = 0) : exitCode(_exitCode)
    {
        if (pid == 0)
            Event<sWindowManagerEvent>({EXIT, this}, WINDOW_MANAGER_PID, false);
        else
            Event<sWindowManagerEvent>({EXIT, this}, pid, false);
    }
} __attribute__((packed));

#endif
