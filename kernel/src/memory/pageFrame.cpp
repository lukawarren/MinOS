#include "memory/pageFrame.h"
#include "memory/memory.h"
#include "cpu/cpu.h"
#include "stdlib.h"

namespace Memory
{
    PageFrame kPageFrame;

    PageFrame::PageFrame(uint32_t* pPageDirectories, uint32_t* pPageTables, uint32_t* pPageBitmaps)
    {
        m_Type = Type::KERNEL;

        m_PageDirectories = pPageDirectories;
        m_PageTables = pPageTables;
        m_PageBitmaps = pPageBitmaps;
    }

    PageFrame::PageFrame(const uint32_t entrypoint, uint32_t codeSize)
    {
        /*
            User page frames need to map kernel code, albeit as a kernel page,
            as in our task-switching code, before the iret, we want to use
            the new cr3 value, but thankfully are still in ring 0 (so we've
            no need to make the interrupt a user-page).

            In an ideal world, page tables in a user-space page frame allocator
            would only be present for page directories actually present, but
            this complicates the code and it could be argued that a flatter
            memory model leads to faster "de-allocation" of this page frame
            upon a task exiting (well, that's my story and I'm sticking to it),
            and as memory is usually in abundance, I see no reason (as of now) 
            to waste hours debugging a crude "JIT" implementation that walks on
            plastic stilts and occasionally spits out the odd paging exception.
        */
       
        // Set type then malloc space for all our stuff
        m_Type = Type::USERSPACE;
        m_PageDirectories = (uint32_t*) kPageFrame.AllocateMemory(sizeof(uint32_t) * NUM_DIRECTORIES);
        m_PageTables = (uint32_t*)kPageFrame.AllocateSwathe(sizeof(uint32_t)*NUM_TABLES*NUM_DIRECTORIES);
        m_PageBitmaps = (uint32_t*)kPageFrame.AllocateMemory(sizeof(uint32_t)*NUM_DIRECTORIES*NUM_TABLES/32);
        
        // Clear pages and setup page directories
        for (uint32_t i = 0; i < NUM_DIRECTORIES; ++i)
            InitPageDirectory(i * DIRECTORY_SIZE);
        
        // Identity-map kernel pages
        for (uint32_t i = 0; i < userspaceBegin / PAGE_SIZE; ++i)
            SetPage(i * PAGE_SIZE, i * PAGE_SIZE, KERNEL_PAGE);

        // Map user-space code pages, starting at 0x40000000 (1GB)
        codeSize = Memory::RoundToNextPageSize(codeSize);
        for (uint32_t i = 0; i < codeSize / PAGE_SIZE; ++i)
            SetPage(entrypoint + i * PAGE_SIZE, 0x40000000, USER_PAGE);
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
        memset(m_PageBitmaps + bitmapNthPage/32, 0, sizeof(uint32_t)*32);
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

        // If user-mode, reflect this change in the kernel page frame too
        if (m_Type == Type::USERSPACE) kPageFrame.SetPageInBitmap(physicalAddress, bAllocated);
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

        // If the page group is double, we're in trouble! (well or anything more than 1 page group).
        // I don't want to spoil such nice code by complicating things, so that belongs to
        // a separate function
        assert(neededPages <= 32);

        // Search through each "group", limited to the confines of physical memory
        for (uint32_t group = 0; group < maxGroups; ++group)
        {
            // Skip if bitmap is full
            uint32_t bitmap = kPageFrame.m_PageBitmaps[group]; // Changes are always reflected in the kernel's bitmap, regardless of if the alloaction belongs to us
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

    void* PageFrame::AllocateSwathe(const uint32_t size)
    {
        // Round size to nearest page and group
        const uint32_t neededPages = RoundToNextPageSize(size) / PAGE_SIZE;
        
        // Running total of pages found - if a new group doesn't have enough room and lets down
        // its precursors there's no need looking back so to that end a simple, fast for loop suffices
        uint32_t remainingPages = neededPages;
        uint32_t startingGroup = 0;

        // Search through each "group", limited to the confines of physical memory
        for (uint32_t group = 0; group < maxGroups; ++group)
        {
            uint32_t bitmap = kPageFrame.m_PageBitmaps[group]; // See AllocateMemory(...) for why it's kPageFrame

            // If we're not the last group
            if (remainingPages > 32)
            {
                // If our group isn't empty, give up our current groups
                if (bitmap + 1 == 0)
                {
                    remainingPages = neededPages;
                    startingGroup = group+1;
                }

                // Otherwise soldier on
                else remainingPages -= 32;
            }

            else
            {
                // If we are in the last group, our remaining pages need to be *at the start* of the group
                const uint32_t mask = (1 << remainingPages) - 1;
                if ((bitmap & mask) == mask)
                {
                    // Now just work out the address and set that number of pages
                    const uint32_t nthPage = startingGroup * 32;
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
                else // Once again, give up otherwise
                {
                    remainingPages = neededPages;
                    startingGroup = group+1;
                }
            }
        }

        assert(false); // Panic!
        return (void*) 0;
    }
    
    void PageFrame::FreeSwathe(const void* physicalAddress, const uint32_t size)
    {

    }
    
    void PageFrame::UsePaging()
    {
        CPU::LoadPageDirectories((uint32_t)m_PageDirectories);
    }
}
