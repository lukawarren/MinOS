#pragma once
#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

#include "multiboot.h"
#include "memory/pageFrame.h"

#define USER_PAGING_OFFSET 0x40000000
#define FRAMEBUFFER_OFFSET 0x30000000

namespace Memory
{
    void Init(const multiboot_info_t* pMultiboot);

    uint32_t GetMaxMemory(const multiboot_info_t* pMultiboot);
    uint32_t RoundToNextPageSize(const uint32_t size);

    extern uint32_t userspaceBegin;
    extern uint32_t maxGroups;
    extern uint32_t nFramebufferPages;
    extern uint32_t framebufferAddress;

    extern "C"
    {
        extern uint32_t __kernel_end;
    }
}

#endif