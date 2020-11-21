#include "paging.h"
#include "mmu.h"
#include "../io/vga.h"

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

    // Allocate enough space for all page tables and page directories
    // Then create pointer to page list
    volatile uint32_t* pageDirectory = (uint32_t*)pagingBegin;
    uint32_t sizeOfAllPageDirectoriesAndTables = (sizeof(uint32_t) * numPages) * (sizeof(uint32_t) * numDirectories);
    Page* pageListArray = (Page*) pagingBegin + sizeOfAllPageDirectoriesAndTables;

    auto AllocatePageDirectory = [&](uint32_t physicalAddress, uint32_t offset, uint32_t flags, bool kernel)
    {
        // Find page directory to be changed - ignore divide by 0
        unsigned int pageDirectoryIndex = (physicalAddress == 0) ? 0 : (physicalAddress / pageDirectorySize);

        // Page tables exist after all page directories
        uint32_t* tables = (uint32_t*) (pagingBegin + (sizeof(uint32_t) * numDirectories) + (sizeof(uint32_t) * pageDirectoryIndex));

        // Fill all tables then fill directory with entry to table
        for (int i = 0; i < 1024; ++i) tables[i] = (i * PAGE_SIZE + physicalAddress + offset) | flags;
        pageDirectory[pageDirectoryIndex] = ((unsigned int)tables) |  flags;

        // Add information to pageListArray
        //for (uint32_t i = physicalAddress / PAGE_SIZE; i < (physicalAddress + pageDirectorySize) / PAGE_SIZE; ++i) 
        //    pageListArray[i] = Page(i * PAGE_SIZE + physicalAddress + offset, allocated, kernel);

        LoadPageDirectory((uint32_t)&pageDirectory[pageDirectoryIndex]);
    };

    auto DeallocatePageDirectory = [&](uint32_t physicalAddress)
    {
        // Find page directory to be changed - ignore divide by 0
        unsigned int pageDirectoryIndex = (physicalAddress == 0) ? 0 : (physicalAddress / pageDirectorySize);

        // Set flags to not present
        pageDirectory[pageDirectoryIndex] = PD_SUPERVISOR(1) | PD_READWRITE(1) | PD_PRESENT(0);
        
        // Update page list array

        LoadPageDirectory((uint32_t)&pageDirectory[pageDirectoryIndex]);
    };

    // Set all pages as empty
    for (uint32_t i = 0; i < 1024; ++i) DeallocatePageDirectory(i * pageDirectorySize);

    // Allocate 32 MB (8 page directories) identity mapped for kernel
    //for (uint32_t i = 0; i < 8; ++i) AllocatePageDirectory(i * pageDirectorySize, 0, PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1), true);
    AllocatePageDirectory(0,                    0, PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1), true);
    AllocatePageDirectory(pageDirectorySize,    0, PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1), true);

    // User space will begin at 0x40000000 (1 GB) virtually
    //AllocatePageDirectory(0x1000000, 0x40000000, PD_PRESENT(1) | PD_READWRITE(0) | PD_SUPERVISOR(0), false);
    
    // Build page list array to keep track of which pages are being used

    EnablePaging();
}