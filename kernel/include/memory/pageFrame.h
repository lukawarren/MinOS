#pragma once
#ifndef PAGE_FRAME_H
#define PAGE_FRAME_H

#include <stdint.h>
#include <stddef.h>

#include "cpu/cpu.h"

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
#define USER_PAGE_READ_ONLY (PD_PRESENT(1) | PD_READWRITE(0) | PD_GLOBALACCESS(1))

namespace Memory
{
    class PageFrame
    {
        public:
            enum class Type
            {
                KERNEL,
                USERSPACE
            };

            inline PageFrame() {}
            PageFrame(uint32_t* pPageDirectories, uint32_t* pPageTables, uint32_t* pPageBitmaps);
            PageFrame(const uint32_t stack, uint32_t stackSize);

            void InitPageDirectory(const uint32_t physicalAddress);

            void SetPage(const uint32_t physicalAddress, const uint32_t virtualAddress, const uint32_t flags);
            void ClearPage(const uint32_t physicalAddress, const uint32_t virtualAddress);
            bool IsPageSet(const uint32_t physicalAddress);

            void* AllocateMemory(const uint32_t size, const uint32_t flags, const uint32_t virtualAddress = 0);
            void FreeMemory(const uint32_t physicalAddress, const uint32_t virtualAddress, const uint32_t size);
            void UnmapMemory(const uint32_t virtualAddress, const uint32_t size);

            void UsePaging();
            uint32_t GetCR3();

            void FreeAllPages();
            uint32_t GetUsedPages();

            template<typename T>
            T GetValueAtVirtualAddress(uint32_t address)
            {
                uint32_t oldCR3 = CPU::GetCR3();

                // De-reference address with our CR3...
                T value;
                UsePaging();
                value = *((T*)address);

                // ...then reload the original one
                CPU::LoadPageDirectories(oldCR3);
                return value;
            }

            uint32_t VirtualToPhysicalAddress(uint32_t address);

        private:

            // Page directories, tables and what-not
            uint32_t* m_PageDirectories;
            uint32_t* m_PageTables;
            uint32_t* m_PageBitmaps;

            Type m_Type;

            void SetPageInBitmap(const uint32_t physicalAddress, const bool bAllocated);
    };

    extern PageFrame kPageFrame;

    extern "C"
    {
        extern uint32_t kernelCR3;
    }
}

#endif