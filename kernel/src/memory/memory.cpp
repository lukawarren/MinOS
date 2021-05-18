#include "memory/memory.h"
#include "stdout/uart.h"
#include "cpu/cpu.h"
#include "kstdlib.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

namespace Memory
{
    uint32_t userspaceBegin;
    uint32_t maxGroups;
    uint32_t nFramebufferPages;
    uint32_t framebufferAddress;

    void Init(const multiboot_info_t* pMultiboot)
    {
        // Get memory bounds
        uint32_t upperBound = GetMaxMemory(pMultiboot);
        uint32_t maxPages = upperBound / PAGE_SIZE;
        maxGroups = maxPages / 32;

        // Allocate space for page tables, page directories and what-not
        uint32_t* pageDirectories = &__kernel_end; // Linker has aligned to nearest 4k
        uint32_t* pageTables = pageDirectories + NUM_DIRECTORIES;
        uint32_t* pageBitmaps = pageTables + NUM_TABLES*NUM_DIRECTORIES;

        // Create page frame
        kPageFrame = PageFrame(pageDirectories, pageTables, pageBitmaps);

        // Bitmap would be number of pages / 32 = 1024 * 1024 / 32 = 32,768 32-bit entries: 1mb
        userspaceBegin = (uint32_t)(pageBitmaps + NUM_DIRECTORIES*NUM_TABLES/32);

        // Clear pages and setup page directories
        for (uint32_t i = 0; i < NUM_DIRECTORIES; ++i)
            kPageFrame.InitPageDirectory(i * DIRECTORY_SIZE);

        // Identity-map kernel pages
        for (uint32_t i = 0; i < userspaceBegin / PAGE_SIZE; ++i)
            kPageFrame.SetPage(i * PAGE_SIZE, i * PAGE_SIZE, KERNEL_PAGE);

        // Enable paging
        kPageFrame.UsePaging();
        CPU::EnablePaging();
    }

    uint32_t GetMaxMemory(const multiboot_info_t* pMultiboot)
    {
        /*
            Attempt to find a "reasonable" contiguous region of memory.
            To that end, enumerate the multiboot's memory map and settle
            on the 2nd (ish) chunck, starting from 0x1000000 (16mb) and
            extending to just before the memory mapped PCI devices (if any)
        */

        multiboot_memory_map_t* pMemoryMap = (multiboot_memory_map_t*)pMultiboot->mmap_addr;
       
        while ((uint32_t)pMemoryMap < pMultiboot->mmap_addr + pMultiboot->mmap_length)
        {
            if (pMemoryMap->addr == 0x100000)
            {
                return (uint32_t)(pMemoryMap->addr + pMemoryMap->len);
            }
            pMemoryMap = (multiboot_memory_map_t*) ((uint32_t)pMemoryMap + pMemoryMap->size + sizeof(pMemoryMap->size));
        }

        return 0;
    }

    uint32_t RoundToNextPageSize(const uint32_t size)
    {
        const uint32_t remainder = size % PAGE_SIZE;
        return (remainder == 0) ? size : size + PAGE_SIZE - remainder;
    }

}

#pragma GCC diagnostic pop
