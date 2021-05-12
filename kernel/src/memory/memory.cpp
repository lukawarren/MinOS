#include "memory/memory.h"
#include "stdout/uart.h"
#include "cpu/cpu.h"
#include "stdlib.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

// Page directories, tables and what-not
static uint32_t* pageDirectories;
static uint32_t* pageTables;
static uint32_t* pageBitmaps;

// Memory bounds
static uint32_t maxPages;
static uint32_t maxGroups;

namespace Memory
{
    void Init(const multiboot_info_t* pMultiboot)
    {
        // Get memory bounds
        const uint32_t upperBound = GetMaxMemory(pMultiboot);
        maxPages = upperBound / PAGE_SIZE;
        maxGroups = maxPages / 32;

        // Allocate space for page tables, page directories and what-not
        pageDirectories = &__kernel_end; // Linker has aligned to nearest 4k
        pageTables = pageDirectories + NUM_DIRECTORIES; // To get rid of warning
        pageBitmaps = pageTables + NUM_TABLES*NUM_DIRECTORIES;

        // Bitmap would be number of pages / 32 = 1024 * 1024 / 32 = 32,768 32-bit entries: 1mb
        const uint32_t userspaceBegin = (uint32_t)(pageBitmaps + 32768);

        // Clear pages and setup page directories
        for (uint32_t i = 0; i < NUM_DIRECTORIES; ++i)
            InitPageDirectory(i * DIRECTORY_SIZE);

        // Identity-map kernel pages
        for (uint32_t i = 0; i < userspaceBegin / PAGE_SIZE; ++i)
            SetPage(i * PAGE_SIZE, i * PAGE_SIZE, KERNEL_PAGE);

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

    static inline uint32_t GetPageDirectoryIndex(const uint32_t physicalAddress)
    {
        return (physicalAddress == 0) ? 0 : (physicalAddress / DIRECTORY_SIZE);
    }

    static inline uint32_t GetPageTableIndex(const uint32_t physicalAddress)
    {
        return (physicalAddress == 0) ? 0 : (physicalAddress / PAGE_SIZE);
    }

    void InitPageDirectory(const uint32_t physicalAddress)
    {
        // Find page directory to be changed - ignore divide by 0
        unsigned int pageDirectoryIndex = GetPageDirectoryIndex(physicalAddress);

        // Set page tables to not present
        uint32_t* pageTable = pageTables + NUM_TABLES*pageDirectoryIndex;
        for (int i = 0; i < NUM_TABLES; ++i) pageTable[i] = PD_PRESENT(0);

        // Set page directory to be user space and present (which is overridden by the tables)
        pageDirectories[pageDirectoryIndex] = (uint32_t)pageTable | USER_PAGE;
        CPU::FlushTLB();

        // Clear bitmap - 1024 pages makes 1024 / 32 = 32 groups
        unsigned int bitmapNthPage = (physicalAddress == 0) ? 0 : (physicalAddress / PAGE_SIZE);
        memset(pageBitmaps + bitmapNthPage, 0, sizeof(uint32_t)*32);
    }

    static void SetPageInBitmap(const uint32_t physicalAddress, const bool bAllocated)
    {
        // Find 32-bit "group" (nth page / 32), then get bit in that page (the remainder)
        unsigned int bitmapNthPage = (physicalAddress == 0) ? 0 : (physicalAddress / PAGE_SIZE);
        unsigned int bitmapIndex = (bitmapNthPage == 0) ? 0 : (bitmapNthPage / 32);
        unsigned int remainder = bitmapNthPage % 32;

        // Set or clear the nth bit
        if  (bAllocated) pageBitmaps[bitmapIndex] |= 1UL << remainder;
        else pageBitmaps[bitmapIndex] &= ~(1UL << remainder);
    }

    void SetPage(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags)
    {
        // Set page table and bitmap
        pageTables[GetPageTableIndex(virtualAddress)] = physicalAddress | flags;
        SetPageInBitmap(physicalAddress, true);
        CPU::FlushTLB();
    }

    void ClearPage(const uint32_t physicalAddress)
    {
        pageTables[GetPageTableIndex(physicalAddress)] = PD_PRESENT(0);
        SetPageInBitmap(physicalAddress, false);
        CPU::FlushTLB();
    }

    bool IsPageSet(const uint32_t physicalAddress)
    {
        unsigned int bitmapNthPage = (physicalAddress == 0) ? 0 : (physicalAddress / PAGE_SIZE);
        unsigned int bitmapIndex = (bitmapNthPage == 0) ? 0 : (bitmapNthPage / 32);
        unsigned int remainder = bitmapNthPage % 32;

        return (pageBitmaps[bitmapIndex] & (1UL << remainder));
    }

    uint32_t RoundToNextPageSize(const uint32_t size)
    {
        const uint32_t remainder = size % PAGE_SIZE;
        return (remainder == 0) ? size : size + PAGE_SIZE - remainder;
    }

    /*
        https://graphics.stanford.edu/~seander/bithacks.html
        "Count the consecutive zero bits (trailing) on the right with multiply and lookup"

        The expression (v & -v) extracts the least significant 1 bit from v. The constant 0x077CB531UL is a de Bruijn sequence, 
        which produces a unique pattern of bits into the high 5 bits for each possible bit position that it is multiplied against.
        When there are no bits set, it returns 0.
    */
    static uint32_t GetPositionOfLeastSignificantBit(const uint32_t number)
    {
        static const int MultiplyDeBruijnBitPosition[32] = 
        {
            0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
            31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
        };

        return MultiplyDeBruijnBitPosition[((uint32_t)((number & -number) * 0x077CB531U)) >> 27];
    }

    void* AllocateMemory(const uint32_t size)
    {
        // Round size to nearest page
        const uint32_t neededPages = RoundToNextPageSize(size) / PAGE_SIZE;

        // If the page group is double, we're in trouble! (well or anything more than 1 page group)
        assert(neededPages <= 32);

        // Search through each "group", limited to the confines of physical memory
        for (uint32_t group = 0; group < maxGroups; ++group)
        {
            // Skip if bitmap is full
            uint32_t bitmap = pageBitmaps[group];
            if (bitmap + 1 == 0) continue;

            // If we need more than 1 page, get fancy
            if (neededPages > 1)
            {
                // Invert the bits
                bitmap = ~bitmap;

                // An empty group (well it's full now cause we inverted it) will break the below code,
                // but luckily it can be skipped, which is faster anyway
                if (bitmap + 1 != 0)
                {
                    // Find if N consecutive bits set: "bitmap & (bitmap >> 1)" for 2 pages, "bitmap & (bitmap >> 1) & (bitmap >> 2)" for 3, etc
                    for (uint32_t nConsecutivePages = 2; (nConsecutivePages <= neededPages && bitmap); ++nConsecutivePages)
                        bitmap &= (bitmap >> (nConsecutivePages-1));

                    // If not, too bad!
                    if (bitmap == 0) continue;
                }

                // Now all we have to do is find the position of the least significant bit
                uint32_t nthBit = GetPositionOfLeastSignificantBit(bitmap);

                // Because the nth bit starts from the right, we don't need to modify the nth bit to account for multiple
                // pages, as memory goes this way: 32nd bit <--------------------- 0th bit

                // Now just work out the address and set that number of pages
                const uint32_t nthPage = group * 32 + nthBit;
                for (uint32_t page = 0; page < neededPages; page++)
                {
                    const uint32_t address = (nthPage + page) * PAGE_SIZE;
                    SetPage(address, address, KERNEL_PAGE);

                    // Zero-out page for security
                    uint32_t* pData = (uint32_t*)address;
                    for (uint32_t i = 0; i < PAGE_SIZE/sizeof(uint32_t); ++i)
                        *(pData + i) = 0;
                }

                return (void*)(PAGE_SIZE * nthPage);
            }
            else
            {
                // Invert for code below (yes, it's a waste, but it does save a while loop)
                bitmap = ~bitmap;

                // Again, find the least significant bit
                uint32_t nthBit = GetPositionOfLeastSignificantBit(bitmap);

                // Again, just work out the address
                const uint32_t address = PAGE_SIZE * (group * 32 + nthBit);
                SetPage(address, address, KERNEL_PAGE);

                // Zero-out page for security
                uint32_t* pData = (uint32_t*)address;
                for (uint32_t i = 0; i < PAGE_SIZE/sizeof(uint32_t); ++i)
                    *(pData + i) = 0;

                return (void*)address; 
            }
            
        }

        assert(false); // Panic!
        return (void*) 0;
    }

    void FreeMemory(const void* physicalAddress, const uint32_t size)
    {
        const uint32_t neededPages = RoundToNextPageSize(size) / PAGE_SIZE;
        for (uint32_t page = 0; page < neededPages; ++page)
        {
            ClearPage((uint32_t)physicalAddress + page*PAGE_SIZE);
        }
    }

}
#pragma GCC diagnostic pop
