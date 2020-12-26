#pragma once
#ifndef SSE_H
#define SSE_E

extern "C"
{
    bool IsSSESupported();
    void EnableSSE();
}

#endif