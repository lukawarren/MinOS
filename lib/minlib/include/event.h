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
    
    Event(const T& data, const uint32_t pid)
    {
        // Send message
        Message message;
        memcpy(message.data, (void*)&data, sizeof(message.data));
        sendmessage(&message, pid);
        
        // Block and wait for ACK
        block();
        removemessage(EVENT_ACK_FILTER);
    }
    
    static Pair<bool, Message> GetMessage()
    {
        // Get message
        Message srcMessage;
        if (!getmessage(&srcMessage))
        {
            // Return if no message is to be found
            return Pair(false, Message {});
        }
        
        // Send ACK
        Message ackMessage;
        strcpy((char*)ackMessage.data, EVENT_ACK);
        sendmessage(&ackMessage, srcMessage.sourcePID);
        
        return Pair(true, srcMessage);
    }
    
    ~Event() {}
    
};

#endif
