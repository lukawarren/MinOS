#include "memory/memory.h"
#include "stdout/uart.h"
#include "cpu/cpu.h"
#include "stdlib.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

extern uint32_t __kernel_end;

// Page directories, tables and what-not
static uint32_t* pageDirectories;
static uint32_t* pageTables;
static uint32_t* pageBitmaps;

namespace Memory
{
    void Init(const multiboot_info_t* pMultiboot)
    {
        // Get size of memory
        const uint32_t upperBound = GetMaxMemory(pMultiboot);
        uint32_t* pFreeMemory = &__kernel_end; // Linker has aligned to nearest 4k
        UART::WriteString("Allocating memory of size ");
        UART::WriteNumber(upperBound);
        UART::WriteString("\n");

        // Allocate space for page tables, page directories and what-not
        pageDirectories = pFreeMemory;
        pageTables = pageDirectories + NUM_DIRECTORIES; // To get rid of warning
        pageBitmaps = pageTables + NUM_TABLES*NUM_DIRECTORIES;

        // Bitmap would be number of pages / 32 = 1024 * 1024 / 32 = 32,768 32-bit entries: 1mb
        const uint32_t userspaceBegin = (uint32_t)(pageBitmaps + 32768);

        // Clear pages
        for (uint32_t i = 0; i < NUM_DIRECTORIES; ++i) DeallocatePageDirectory(i * DIRECTORY_SIZE, USER_PAGE);

        // Identity-map kernel pages
        for (uint32_t i = 0; i < userspaceBegin / PAGE_SIZE; ++i)
        {
            SetPage(i * PAGE_SIZE, i * PAGE_SIZE, KERNEL_PAGE);
        }

        // Enable paging
        CPU::LoadPageDirectories((uint32_t)pageDirectories);
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

    void DeallocatePageDirectory(const uint32_t virtualAddress, const uint32_t flags)
    {
        // Pages can be allocated easier if we leave the page directory
        // allocated but deallocate all page tables
        assert((flags & 0b1) == PD_PRESENT(1));

        // Find page directory to be changed - ignore divide by 0
        unsigned int pageDirectoryIndex = (virtualAddress == 0) ? 0 : (virtualAddress / DIRECTORY_SIZE);

        // Set page tables
        uint32_t* pageTable = pageTables + NUM_TABLES*pageDirectoryIndex;
        for (int i = 0; i < NUM_TABLES; ++i) pageTable[i] = PD_PRESENT(0);

        // Set page directory
        pageDirectories[pageDirectoryIndex] = (uint32_t)pageTable | flags;

        CPU::FlushTLB();

        // Clear bitmap - 1024 pages makes 1024 / 32 = 32 groups
        unsigned int bitmapNthPage = (virtualAddress == 0) ? 0 : (virtualAddress / PAGE_SIZE);
        memset(pageBitmaps + bitmapNthPage, 0, sizeof(uint32_t)*32);
    }

    void SetPage(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags)
    {
        // Find page table - ignore divide by 0
        unsigned int pageTableIndex = (virtualAddress == 0) ? 0 : (virtualAddress / PAGE_SIZE);
        uint32_t* pageTable = pageTables + pageTableIndex;

        // Fill table and flush TLB
        *pageTable = physicalAddress | flags;
        CPU::FlushTLB();

        // Find 32-bit "group" (nth page / 32), then get bit in that page (the remainder)
        unsigned int bitmapNthPage = (virtualAddress == 0) ? 0 : (virtualAddress / PAGE_SIZE);
        unsigned int bitmapIndex = (bitmapNthPage == 0) ? 0 : (bitmapNthPage / 32);
        unsigned int remainder = bitmapNthPage % 32;

        // Set the nth bit
        pageBitmaps[bitmapIndex] |= 1UL << remainder;
    }

    void ClearPage(const uint32_t virtualAddress)
    {
        // Find page table - ignore divide by 0
        unsigned int pageTableIndex = (virtualAddress == 0) ? 0 : (virtualAddress / PAGE_SIZE);
        uint32_t* pageTable = pageTables + pageTableIndex;

        // Fill table and flush TLB
        *pageTable = PD_PRESENT(0);
        CPU::FlushTLB();

        // Find 32-bit "group" (nth page / 32), then get bit in that page (the remainder)
        unsigned int bitmapNthPage = (virtualAddress == 0) ? 0 : (virtualAddress / PAGE_SIZE);
        unsigned int bitmapIndex = (bitmapNthPage == 0) ? 0 : (bitmapNthPage / 32);
        unsigned int remainder = bitmapNthPage % 32;

        // Clear the nth bit
        pageBitmaps[bitmapIndex] &= ~(1UL << remainder);
    }

    static uint32_t RoundToNextPageSize(const uint32_t size)
    {
        const uint32_t remainder = size % PAGE_SIZE;
        return (remainder == 0) ? size : size + PAGE_SIZE - remainder;
    }

    uint32_t AllocatePage(const uint32_t size)
    {
        // Round size to nearest page
        const uint32_t neededPages = RoundToNextPageSize(size) / PAGE_SIZE;
        const uint32_t pagesAsBinary = (1 << neededPages) - 1;

        // Search through each "group"
        for (int group = 0; group < NUM_DIRECTORIES * NUM_TABLES; ++group)
        {
            // If there remains any free bits (pages)
            if (pageBitmaps[group] + 1 != 0)
            {
                uint32_t bitmap = pageBitmaps[group];
                uint32_t bitCounter = 0;

                // Continuously shift to the right and check if all
                // necessary bits (pages) are 0
                while (bitCounter < 32)
                {
                    if ((bitmap & pagesAsBinary) == 0)
                    {
                        // We've found the memory, get the address
                        const uint32_t address = (group * 32 + bitCounter) * PAGE_SIZE;
                        SetPage(address, address, KERNEL_PAGE);
                        return address;
                    }
                    bitmap >>= 1;
                    bitCounter++;
                }
            }
        }

        assert(false); // Panic!
        return -1;
    }

}
#pragma GCC diagnostic pop
