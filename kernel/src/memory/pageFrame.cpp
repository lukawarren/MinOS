#include "memory/pageFrame.h"
#include "filesystem/filesystem.h"
#include "io/gfx/framebuffer.h"
#include "memory/memory.h"
#include "cpu/cpu.h"
#include "kstdlib.h"

namespace Memory
{
    PageFrame kPageFrame;
    uint32_t kernelCR3 = 0;

    PageFrame::PageFrame(uint32_t* pPageDirectories, uint32_t* pPageTables, uint32_t* pPageBitmaps)
    {
        m_Type = Type::KERNEL;

        m_PageDirectories = pPageDirectories;
        m_PageTables = pPageTables;
        m_PageBitmaps = pPageBitmaps;

        kernelCR3 = GetCR3();
    }

    PageFrame::PageFrame(const uint32_t stack, uint32_t stackSize)
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
        m_PageDirectories = (uint32_t*) kPageFrame.AllocateMemory(sizeof(uint32_t) * NUM_DIRECTORIES, KERNEL_PAGE);
        m_PageTables = (uint32_t*)kPageFrame.AllocateMemory(sizeof(uint32_t)*NUM_TABLES*NUM_DIRECTORIES, KERNEL_PAGE);
        m_PageBitmaps = (uint32_t*)kPageFrame.AllocateMemory(sizeof(uint32_t)*NUM_DIRECTORIES*NUM_TABLES/32, KERNEL_PAGE);
        
        // Clear pages and setup page directories
        for (uint32_t i = 0; i < NUM_DIRECTORIES; ++i)
            InitPageDirectory(i * DIRECTORY_SIZE);
        
        // Identity-map kernel pages, but do *not* set them in the bitmap (they're already reflected in the kernel)
        for (uint32_t i = 0; i < userspaceBegin / PAGE_SIZE; ++i)
            SetPage(i * PAGE_SIZE, i * PAGE_SIZE, KERNEL_PAGE, false);

        // Map stack pages
        stackSize = Memory::RoundToNextPageSize(stackSize);
        for (uint32_t i = 0; i < stackSize / PAGE_SIZE; ++i)
            SetPage(stack + i * PAGE_SIZE, stack + i * PAGE_SIZE, USER_PAGE);
    }

    static inline uint32_t GetPageDirectoryIndex(const uint32_t virtualAddress)
    {
        return (virtualAddress == 0) ? 0 : (virtualAddress / DIRECTORY_SIZE);
    }

    static inline uint32_t GetPageTableIndex(const uint32_t virtualAddress)
    {
        return (virtualAddress == 0) ? 0 : (virtualAddress / PAGE_SIZE);
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

    void PageFrame::SetPage(const uint32_t physicalAddress, const uint32_t virtualAddress, const uint32_t flags, const bool bReflectInBitmap)
    {
        assert((physicalAddress & 0xfff) == 0); // Assert page aligned
        
        // Set page table and bitmap
        m_PageTables[GetPageTableIndex(virtualAddress)] = physicalAddress | flags;
        if (bReflectInBitmap) SetPageInBitmap(physicalAddress, true);

        // Identity map *in kernel* so that user allocations can be modified on their behalf
        if (this != &kPageFrame)
            kPageFrame.m_PageTables[GetPageTableIndex(physicalAddress)] = physicalAddress | flags;

        CPU::FlushTLB();
    }

    void PageFrame::ClearPage(const uint32_t physicalAddress, const uint32_t virtualAddress)
    {
        m_PageTables[GetPageTableIndex(virtualAddress)] = PD_PRESENT(0);
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

    void* PageFrame::AllocateMemory(const uint32_t size, const uint32_t flags, const uint32_t virtualAddress)
    {
        // Round size to nearest page
        const uint32_t neededPages = RoundToNextPageSize(size) / PAGE_SIZE;

        // If less than 1 page is needed, then we're looking for a consecutive group of 0's
        // within 1 bitmap group, but if we need more than 1 group, we're just looking for
        // N number of full groups, plus a following group with the first N bits free.
        if (neededPages <= 32)
        {
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
                        if (virtualAddress == 0) SetPage(address, address, flags); // Identity mapping
                        
                        // Non-identy mapping
                        else
                        {
                            const uint32_t mappedAddress = virtualAddress + page*PAGE_SIZE;
                            SetPage(address, mappedAddress, flags);

                            // Zero-out page for security
                            uint32_t* pData = (uint32_t*)address;
                            for (uint32_t i = 0; i < PAGE_SIZE/sizeof(uint32_t); ++i)
                                *(pData + i) = 0;
                        }
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
                    if (virtualAddress == 0) SetPage(address, address, flags); // Identity maping
                    else SetPage(address, virtualAddress, flags); // Non-identity mapping

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
        else
        {
            // Running total of pages found - if a new group doesn't have enough room and lets down
            // its precursors there's no need looking back so to that end a simple, fast for loop suffices
            uint32_t remainingPages = neededPages;
            uint32_t startingGroup = 0;

            // Search through each "group", limited to the confines of physical memory
            for (uint32_t group = startingGroup; group < maxGroups; ++group)
            {
                uint32_t bitmap = kPageFrame.m_PageBitmaps[group]; // See AllocateMemory(...) for why it's kPageFrame
                
                // If we're not the last group
                if (remainingPages > 32)
                {
                    // If our group isn't empty, give up our current groups
                    if (bitmap != 0)
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
                    if ((~bitmap & mask) == mask)
                    {
                        // Now just work out the address and set that number of pages
                        const uint32_t nthPage = startingGroup * 32;
                        for (uint32_t page = 0; page < neededPages; page++)
                        {
                            const uint32_t address = (nthPage + page) * PAGE_SIZE;

                            if (virtualAddress == 0) SetPage(address, address, flags); // Identity mapping
                            
                            // Non-identy mapping
                            else
                            {
                                const uint32_t mappedAddress = virtualAddress + page*PAGE_SIZE;
                                SetPage(address, mappedAddress, flags);
                            }

                            // Zero-out page for security
                            uint32_t* pData = (uint32_t*)address;
                            for (uint32_t i = 0; i < PAGE_SIZE/sizeof(uint32_t); ++i)
                                *(pData + i) = 0;
                        }
                        
                        return (void*)(PAGE_SIZE * nthPage);
                    }
                    else // Once again, give up otherwise, but repeat this iteration
                    {
                        remainingPages = neededPages;
                        startingGroup = group;
                        group--;
                    }
                }
            }

            assert(false); // Panic!
            return (void*) 0;
        }
    }

    void PageFrame::FreeMemory(const uint32_t physicalAddress, const uint32_t virtualAddress, const uint32_t size)
    {
        const uint32_t neededPages = RoundToNextPageSize(size) / PAGE_SIZE;
        for (uint32_t page = 0; page < neededPages; ++page)
        {
            ClearPage((uint32_t)physicalAddress + page*PAGE_SIZE, virtualAddress + page*PAGE_SIZE);
        }
    }
    
    void PageFrame::UsePaging()
    {
        CPU::LoadPageDirectories((uint32_t)m_PageDirectories);
    }

    uint32_t PageFrame::GetCR3()
    {
        return (uint32_t)m_PageDirectories;
    }

    void PageFrame::FreeAllPages()
    {
        assert(m_Type == Type::USERSPACE);

        // Go through each page in *our* bitmap and free that page, *ignored mapping* (as non-identity mapping might make us skip over pages!)
        // We don't actually unmap pages, because at this point we're a dead man walking, so who cares?!
        for (uint32_t page = 0; page < NUM_DIRECTORIES*NUM_TABLES; ++page)
        {
            const uint32_t address = page*PAGE_SIZE;
            if (IsPageSet(address)) SetPageInBitmap(address, false);
        }

        assert(GetUsedPages() == 0);

        // Free paging structures
        kPageFrame.FreeMemory((uint32_t)m_PageDirectories,  (uint32_t)m_PageDirectories,    sizeof(uint32_t)*NUM_DIRECTORIES);
        kPageFrame.FreeMemory((uint32_t)m_PageTables,       (uint32_t)m_PageTables,         sizeof(uint32_t)*NUM_TABLES*NUM_DIRECTORIES);
        kPageFrame.FreeMemory((uint32_t)m_PageBitmaps,      (uint32_t)m_PageBitmaps,        sizeof(uint32_t)*NUM_DIRECTORIES*NUM_TABLES/32);
    }

    static uint32_t GetNumberOfBitsSet(uint32_t i)
    {
        // GCC 10 will recognise this if we use "-O3 -march=nehalem -mtune=skylake" (any Intel CPU after 2008 should work),
        // and replace it with a single instruction, "popcnt"
        i = i - ((i >> 1) & 0x55555555);        // add pairs of bits
        i = (i & 0x33333333) + ((i >> 2) & 0x33333333);  // quads
        i = (i + (i >> 4)) & 0x0F0F0F0F;        // groups of 8
        return (i * 0x01010101) >> 24;          // horizontal sum of bytes
    }

    uint32_t PageFrame::GetUsedPages()
    {
        uint32_t pages = 0;
        
        for (uint32_t page = 0; page < NUM_DIRECTORIES*NUM_TABLES / 32; ++page)
        {
            const uint32_t bitmap = m_PageBitmaps[page];
            pages += GetNumberOfBitsSet(bitmap);
        }
        
        return pages;
    }

    uint32_t PageFrame::VirtualToPhysicalAddress(uint32_t address)
    {
        // Work out offset from page aligned address
        const uint32_t offset = address % PAGE_SIZE;

        // Return appropriate entry
        return (m_PageTables[GetPageTableIndex(address)] & 0b11111111111111111111000000000000) + offset;
    }

}
