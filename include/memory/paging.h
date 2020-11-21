#pragma once
#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>

#include "mmu.h"

#define PAGE_SIZE 0x1000

extern uint32_t __kernel_end;

/*
    A page will always be aligned to 4kb,
    so that the last three bits will always
    be zero, leaving space for flags
*/
struct Page
{
    uint32_t pageIdentifier;

    Page(uint32_t virtualAddress, bool allocated, bool kernel)
    {
        pageIdentifier = virtualAddress;
        pageIdentifier |= (allocated & 0b1) << 1;
        pageIdentifier |= (kernel & 0b1);
    }

    inline bool IsAllocated() { return pageIdentifier & 0b10; }
    inline bool IsKernel() { return pageIdentifier & 0b1; }

    inline void SetAllocated() { pageIdentifier |= (1 << 1); }
    inline void SetKernel() { pageIdentifier |= 1; }

    inline void ClearAllocated() { pageIdentifier &= ~(1 << 1); }
    inline void ClearKernel() { pageIdentifier &= ~(1); }

    inline uint32_t GetAddress() { return pageIdentifier & 0b11111111111111111111111111111000; }
};

void InitPaging(const uint32_t maxAddress);

#endif