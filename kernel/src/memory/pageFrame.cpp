#include "memory/pageFrame.h"
#include "memory/memory.h"
#include "cpu/cpu.h"
#include "stdlib.h"

namespace Memory
{
    PageFrame::PageFrame(uint32_t* pPageDirectories, uint32_t* pPageTables, uint32_t* pPageBitmaps, 
                            uint32_t nMaxPages, uint32_t nMaxGroups)
    {
        m_Type = Type::KERNEL;

        m_PageDirectories = pPageDirectories;
        m_PageTables = pPageTables;
        m_PageBitmaps = pPageBitmaps;

        m_nMaxPages = nMaxPages;
        m_nMaxGroups = nMaxGroups;
    }

    static inline uint32_t GetPageDirectoryIndex(const uint32_t physicalAddress)
    {
        return (physicalAddress == 0) ? 0 : (physicalAddress / DIRECTORY_SIZE);
    }

    static inline uint32_t GetPageTableIndex(const uint32_t physicalAddress)
    {
        return (physicalAddress == 0) ? 0 : (physicalAddress / PAGE_SIZE);
    }

    void PageFrame::InitPageDirectory(const uint32_t physicalAddress)
    {
        // Find page directory to be changed - ignore divide by 0
        unsigned int pageDirectoryIndex = GetPageDirectoryIndex(physicalAddress);

        // Set page tables to not present
        uint32_t* pageTable = m_PageTables + NUM_TABLES*pageDirectoryIndex;
        for (int i = 0; i < NUM_TABLES; ++i) pageTable[i] = PD_PRESENT(0);

        // Set page directory to be user space and present (which is overridden by the tables)
        m_PageDirectories[pageDirectoryIndex] = (uint32_t)pageTable | USER_PAGE;
        CPU::FlushTLB();

        // Clear bitmap - 1024 pages makes 1024 / 32 = 32 groups
        unsigned int bitmapNthPage = (physicalAddress == 0) ? 0 : (physicalAddress / PAGE_SIZE);
        memset(m_PageBitmaps + bitmapNthPage, 0, sizeof(uint32_t)*32);
    }

    void PageFrame::SetPageInBitmap(const uint32_t physicalAddress, const bool bAllocated)
    {
        // Find 32-bit "group" (nth page / 32), then get bit in that page (the remainder)
        unsigned int bitmapNthPage = (physicalAddress == 0) ? 0 : (physicalAddress / PAGE_SIZE);
        unsigned int bitmapIndex = (bitmapNthPage == 0) ? 0 : (bitmapNthPage / 32);
        unsigned int remainder = bitmapNthPage % 32;

        // Set or clear the nth bit
        if  (bAllocated) m_PageBitmaps[bitmapIndex] |= 1UL << remainder;
        else m_PageBitmaps[bitmapIndex] &= ~(1UL << remainder);
    }

    void PageFrame::SetPage(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags)
    {
        // Set page table and bitmap
        m_PageTables[GetPageTableIndex(virtualAddress)] = physicalAddress | flags;
        SetPageInBitmap(physicalAddress, true);
        CPU::FlushTLB();
    }

    void PageFrame::ClearPage(const uint32_t physicalAddress)
    {
        m_PageTables[GetPageTableIndex(physicalAddress)] = PD_PRESENT(0);
        SetPageInBitmap(physicalAddress, false);
        CPU::FlushTLB();
    }

    bool PageFrame::IsPageSet(const uint32_t physicalAddress)
    {
        unsigned int bitmapNthPage = (physicalAddress == 0) ? 0 : (physicalAddress / PAGE_SIZE);
        unsigned int bitmapIndex = (bitmapNthPage == 0) ? 0 : (bitmapNthPage / 32);
        unsigned int remainder = bitmapNthPage % 32;

        return (m_PageBitmaps[bitmapIndex] & (1UL << remainder));
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

    void* PageFrame::AllocateMemory(const uint32_t size)
    {
        // Round size to nearest page
        const uint32_t neededPages = RoundToNextPageSize(size) / PAGE_SIZE;

        // If the page group is double, we're in trouble! (well or anything more than 1 page group)
        assert(neededPages <= 32);

        // Search through each "group", limited to the confines of physical memory
        for (uint32_t group = 0; group < m_nMaxGroups; ++group)
        {
            // Skip if bitmap is full
            uint32_t bitmap = m_PageBitmaps[group];
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

    void PageFrame::FreeMemory(const void* physicalAddress, const uint32_t size)
    {
        const uint32_t neededPages = RoundToNextPageSize(size) / PAGE_SIZE;
        for (uint32_t page = 0; page < neededPages; ++page)
        {
            ClearPage((uint32_t)physicalAddress + page*PAGE_SIZE);
        }
    }

    void PageFrame::UsePaging()
    {
        CPU::LoadPageDirectories((uint32_t)m_PageDirectories);
    }
}