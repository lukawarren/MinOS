#pragma once
#ifndef MINLIB_EVENT_H
#define MINLIB_EVENT_H

#include <stdlib.h>
#include <stddef.h>

#include "pair.h"

#define EVENT_ACK "ACK"
#define EVENT_ACK_FILTER uint32_t(EVENT_ACK[0] << 24 | EVENT_ACK[1] << 16 | EVENT_ACK[2] << 8 | EVENT_ACK[3])

template<typename T = int>
class Event
{
    static_assert(sizeof(T) <= sizeof(sMessage) - sizeof(uint32_t));
    
public:
    Event() {}
    
    Event(const T& data, const uint32_t pid, bool bBlock = true)
    {
        // Send message and block until ACK
        Message message;
        memcpy(message.data, (void*)&data, sizeof(message.data));
        
        if (bBlock)
        {
            sendmessageuntil(&message, pid, EVENT_ACK_FILTER);
            removemessage(EVENT_ACK_FILTER);
        }
        else sendmessage(&message, pid);
    }
    
    static Pair<bool, Message> GetMessage(bool bBlocked = false)
    {
        // Get message
        Message srcMessage;
        if (!getmessage(&srcMessage))
        {
            // Return if no message is to be found
            return Pair(false, Message {});
        }
        
        if (bBlocked)
        {
            // Send ACK
            Message ackMessage;
            strcpy((char*)ackMessage.data, EVENT_ACK);
            sendmessage(&ackMessage, srcMessage.sourcePID);
        }
    
        return Pair(true, srcMessage);
    }
    
    ~Event() {}
    
};

#endif
