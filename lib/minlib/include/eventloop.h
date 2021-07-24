#pragma once
#ifndef MINLIB_EVENT_LOOP_H
#define MINLIB_EVENT_LOOP_H

#include <stdlib.h>
#include <stddef.h>

template<typename T>
class EventLoop
{
public:

    template<typename Function>
    EventLoop(Function function, const bool bBlock = true)
    {
        bool bRunning = true;
        while (bRunning)
        {
            // Block until next message received
            if (bBlock) block();
            
            // Call message handler for message (if any)
            const Pair<bool, Message> message = Event<>::GetMessage();
            bRunning = function(reinterpret_cast<const T&>(message.m_second.data), message.m_first);
        }
    }

};

#endif
