#pragma once
#ifndef SSE_COPY_H
#define SSE_COPY_H

extern "C"
{
    void MemcpySSE(void* dest, const void* src, const uint32_t size);
}

#endif