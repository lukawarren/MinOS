#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"

#define DIRECTORY_SIZE 0x400000
#define PAGE_SIZE 0x1000

#define NUM_DIRECTORIES 1024
#define NUM_TABLES 1024

#define PD_PRESENT(x)           ((x & 0b1))
#define PD_READWRITE(x)         ((x & 0b1) << 1)
#define PD_GLOBALACCESS(x)      ((x & 0b1) << 2)

#define KERNEL_PAGE     (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(0))
#define USER_PAGE       (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(1))
#define USER_DIRECTORY  (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(1))

namespace Memory
{
    void Init(const multiboot_info_t* pMultiboot);
    uint32_t GetMaxMemory(const multiboot_info_t* pMultiboot);

    void InitPageDirectory(const uint32_t virtualAddress);

    void SetPage(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags);
    void ClearPage(const uint32_t address);

    void* AllocateMemory(const uint32_t size);
    void FreeMemory(const void* address, const uint32_t size);
}

#endif