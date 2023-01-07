/*
    Differs from the normal window.h in that it is solely
    client-orientated, wrapping messages around a C++ class.
*/

#pragma once
#ifndef MINWM_SERVER
#include "client/types.h"
#include "messages.h"
#include <minlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

class Window
{
public:
    Window(const char* title, const Size size)
    {
        // Create framebuffer and share it with PID 1 (minwm)
        size_t length = sizeof(uint32_t) * size.x * size.y;
        framebuffer = (uint32_t*) malloc(length);
        assert(share_memory((size_t)framebuffer, length, 1) == 0);

        CreateWindowMessage message = {};
        message.pid = 1;
        message.id = CREATE_WINOW_MESSAGE;
        message.width = size.x;
        message.height = size.y;
        message.framebuffer = framebuffer;
        strncpy((char*)message.title, title, sizeof(message.title));
        send_message((Message*)&message);
    }

    void set_title(const char* title)
    {
        SetWindowTitleMessage message = {};
        message.pid = 1;
        message.id = SET_WINDOW_TITLE_MESSAGE;
        strncpy((char*)message.title, title, sizeof(message.title));
        send_message((Message*)&message);
    }

    uint32_t* framebuffer;
};

#endif