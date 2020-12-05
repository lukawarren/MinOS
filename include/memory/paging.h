#pragma once
#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>

#include "mmu.h"

extern uint32_t __kernel_end;

#define KERNEL_PAGE     (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(0))
#define USER_PAGE       (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(1))
#define USER_DIRECTORY  (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(1))


/*
    A page will always be aligned to 4kb,
    so that the last three bits will always
    be zero, leaving space for flags
*/
struct Page
{
    uint32_t pageIdentifier;

    Page(uint32_t physicalAddress, bool allocated, bool kernel)
    {
        pageIdentifier = physicalAddress;
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

void AllocatePage(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags, bool kernel);
void DeallocatePage(uint32_t physicalAddress);

void AllocatePageDirectory(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags,  bool kernel);
void DeallocatePageDirectory(uint32_t physicalAddress, uint32_t flags);

void* kmalloc(uint32_t bytes, uint32_t flags = KERNEL_PAGE);
void  kfree(void* ptr, uint32_t bytes);

void PrintPaging();

#endif