#pragma once
#ifndef PAGE_FRAME_H
#define PAGE_FRAME_H

#include <stdint.h>
#include <stddef.h>

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
    class PageFrame
    {
        public:
            enum class Type
            {
                KERNEL,
                USERSPACE
            };

            inline PageFrame() {}
            PageFrame(uint32_t* pPageDirectories, uint32_t* pPageTables, uint32_t* pPageBitmaps, uint32_t nMaxPages, uint32_t nMaxGroups);

            void InitPageDirectory(const uint32_t physicalAddress);

            void SetPage(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags);
            void ClearPage(const uint32_t physicalAddress);
            bool IsPageSet(const uint32_t physicalAddress);

            void* AllocateMemory(const uint32_t size);
            void FreeMemory(const void* physicalAddress, const uint32_t size);

            void UsePaging();

        private:

            // Page directories, tables and what-not
            uint32_t* m_PageDirectories;
            uint32_t* m_PageTables;
            uint32_t* m_PageBitmaps;

            // Memory bounds
            uint32_t m_nMaxPages;
            uint32_t m_nMaxGroups;

            Type m_Type;

            void SetPageInBitmap(const uint32_t physicalAddress, const bool bAllocated);
    };
}

#endif