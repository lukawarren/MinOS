#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"
#include "memory/pageFrame.h"

#define DIRECTORY_SIZE 0x400000
#define PAGE_SIZE 0x1000

#define NUM_DIRECTORIES 1024
#define NUM_TABLES 1024

#define PD_PRESENT(x)           ((x & 0b1))
#define PD_READWRITE(x)         ((x & 0b1) << 1)
#define PD_GLOBALACCESS(x)      ((x & 0b1) << 2)

#define KERNEL_PAGE         (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(0))
#define USER_PAGE           (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(1))
#define USER_DIRECTORY      (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(1))
#define USER_PAGE_READ_ONLY (PD_PRESENT(1) | PD_READWRITE(0) | PD_GLOBALACCESS(1))

#define USER_PAGING_OFFSET 0x40000000

namespace Memory
{
    void Init(const multiboot_info_t* pMultiboot);

    uint32_t GetMaxMemory(const multiboot_info_t* pMultiboot);
    uint32_t RoundToNextPageSize(const uint32_t size);

    extern uint32_t userspaceBegin;
    extern uint32_t maxGroups;

    extern "C"
    {
        extern uint32_t __kernel_end;
    }
}

#endif