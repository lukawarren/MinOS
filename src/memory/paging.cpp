#include "paging.h"
#include "../io/vga.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

constexpr uint32_t pageDirectorySize = 0x400000;

void InitPaging(const uint32_t maxAddress)
{
    /*
        Whilst the page table, page directories, etc could be much smaller on systems with less
        physical RAM, I have decided to have enough space to address the maximum 4GB so that the
        kernel will be a similar size on all systems, making identity-mapped kernel space and its
        affected user space much easier. That means that paging takes 16 MB of memory, followed by
        a further 4 MB for the page list array (I think).
    */

    const uint32_t numDirectories = 1024;
    const uint32_t numPages = 1024;

    // Knowing memory size will allow for allocation in pageListArray later
    uint32_t pagingBegin = (uint32_t)(&__kernel_end); // Address from linker is aligned to nearest 4K
    uint32_t memorySize = maxAddress - pagingBegin;
    uint32_t maxPhysicalPages = memorySize / PAGE_SIZE;

    // Allocate enough space for all page tables and page directories, then create pointer to page list
    // uint32_t pageDirectories[numDirectories], uint32_t pageTables[numPages*numDirectories]
    uint32_t* pageDirectories = (uint32_t*)pagingBegin;
    uint32_t* pageTables = pageDirectories + numDirectories;
    //Page* pageListArray = (Page*) pageTables + numDirectories*numPages; 

    auto AllocatePageDirectory = [&](uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags, bool kernel)
    {
        // Find page directory to be changed - ignore divide by 0
        unsigned int pageDirectoryIndex = (physicalAddress == 0) ? 0 : (physicalAddress / pageDirectorySize);

        // Get page directory and page tables
        uint32_t* pageDirectory = &pageDirectories[pageDirectoryIndex];
        uint32_t* pageTable = pageTables + numPages*pageDirectoryIndex;

        // Fill all tables then fill directory with entry to table
        for (int i = 0; i < 1024; ++i) pageTable[i] = (i * PAGE_SIZE + virtualAddress) | flags;
        *pageDirectory = (uint32_t)pageTable | flags;

        // TODO: Add information to pageListArray

        LoadPageDirectory((uint32_t)pageDirectory);
    };

    auto DeallocatePageDirectory = [&](uint32_t physicalAddress)
    {
        // Find page directory to be changed - ignore divide by 0
        unsigned int pageDirectoryIndex = (physicalAddress == 0) ? 0 : (physicalAddress / pageDirectorySize);

        // Clear page directory
        pageDirectories[pageDirectoryIndex] = (PD_SUPERVISOR(1) | PD_READWRITE(1) | PD_PRESENT(0));
        
        // TODO: Update page list array

        LoadPageDirectory((uint32_t)&pageDirectories[pageDirectoryIndex]);
    };

    // Set all pages as empty
    for (uint32_t i = 0; i < 1024; ++i) DeallocatePageDirectory(i * pageDirectorySize);

    // Allocate 32 MB (8 page directories) identity mapped for kernel
    //for (uint32_t i = 0; i < 8; ++i) AllocatePageDirectory(i * pageDirectorySize, 0, PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1), true);
    AllocatePageDirectory(0,                    0,                  PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1), true);
    AllocatePageDirectory(pageDirectorySize,    pageDirectorySize,  PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1), true);

    // User space will begin at 0x40000000 (1 GB) virtually, but physically at (what is currently) 16MB
    AllocatePageDirectory(0x1000000, 0x40000000, PD_PRESENT(1) | PD_READWRITE(0) | PD_SUPERVISOR(0), false);
    
    // TODO: Build page list array to keep track of which pages are being used

    EnablePaging();
}

#pragma GCC diagnostic pop