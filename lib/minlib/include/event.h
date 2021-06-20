#pragma once
#ifndef MINLIB_EVENT_H
#define MINLIB_EVENT_H

#include <stdlib.h>
#include <stddef.h>

#include "pair.h"

#define EVENT_ACK "ACK"

template<typename T = int>
class Event
{
    static_assert(sizeof(T) <= sizeof(sMessage) - sizeof(uint32_t));
    
public:
    Event() {}
    
    Event(const T& data, const uint32_t pid, bool bWaitForACK = true)
    {
        // Send message
        Message message;
        memcpy(message.data, (void*)&data, sizeof(message.data));
        sendmessage(&message, pid);
        
        if (!bWaitForACK) return;
        
        // Block and wait for ACK
        block();
        popmessage();
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
