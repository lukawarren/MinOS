#include "paging.h"
#include "../gfx/vga.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"

/*
    Whilst the page table, page directories, etc could be much smaller on systems with less
    physical RAM, I have decided to have enough space to address the maximum 4GB so that the
    kernel will be a similar size on all systems, making identity-mapped kernel space and its
    affected user space much easier. That means that paging takes 16 MB of memory, followed by
    a further 4 MB for the page list array (I think).
*/

constexpr uint32_t pageDirectorySize = 0x400000;
constexpr uint32_t pageSize = 0x1000;

const uint32_t numDirectories = 1024;
const uint32_t numPages = 1024;

static uint32_t* pageDirectories;
static uint32_t* pageTables;
static Page* pageListArray;

void InitPaging(const uint32_t maxAddress)
{
    // Knowing memory size will allow for allocation in pageListArray later
    uint32_t pagingBegin = (uint32_t)(&__kernel_end); // Address from linker is aligned to nearest 4K
    uint32_t memorySize = maxAddress - pagingBegin;
    uint32_t maxPhysicalPages = memorySize / pageSize;

    // Allocate enough space for all page tables and page directories, then create pointer to page list
    // uint32_t pageDirectories[numDirectories], uint32_t pageTables[numPages*numDirectories]
    pageDirectories = (uint32_t*)pagingBegin;
    pageTables = pageDirectories + numDirectories;
    pageListArray = (Page*) pageTables + numDirectories*numPages; 

    // Set all pages as empty and fill them with correct values 
    for (uint32_t i = 0; i < 1024; ++i) DeallocatePageDirectory(i * pageDirectorySize);

    // Allocate enough pages to cover memory usage of above memory management
    // Should already be aligned to nearest 4kb
    uint32_t kernelMemorySoFar = (uint32_t)pageListArray + sizeof(uint32_t)*numPages*sizeof(uint32_t)*numDirectories;
    uint32_t pagesToAllocate = (kernelMemorySoFar - pagingBegin) / pageSize;
    for (uint32_t i = 0; i < pagesToAllocate; ++i) AllocatePage(i * pageSize, i * pageSize, PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1), true);

    // User space will begin at 0x40000000 (1 GB) virtually, but physically at (what is currently) 16MB
    // At the time of writing it isn't used, but is allocated as a "token gesture", and to verify the mappping does indeed work.
    //AllocatePageDirectory(0x1000000, 0x40000000, PD_PRESENT(1) | PD_READWRITE(0) | PD_SUPERVISOR(0), false);  

    // Allocate pages for VGA framebuffer, after aligning it
    uint32_t aligendFramebufferAddress = (uint32_t)VGA_framebuffer.address & ~(0xFFFF);
    uint32_t framebufferAlignmentDifference = (uint32_t)VGA_framebuffer.address - aligendFramebufferAddress;
    uint32_t framebufferPages = (sizeof(uint32_t) * VGA_framebuffer.width * VGA_framebuffer.height) / pageSize;
    for (uint32_t i = 0; i < framebufferPages; ++i)
        AllocatePage(aligendFramebufferAddress + i * pageSize, kernelMemorySoFar + i*pageSize, PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1), true);
    VGA_framebuffer.address = (uint32_t*)(kernelMemorySoFar + framebufferAlignmentDifference);

    LoadPageDirectories((uint32_t)pageDirectories);
    EnablePaging();

    VGA_printf("[Success] ", false, VGA_COLOUR_LIGHT_GREEN);
    VGA_printf("Allocated page frame - allocated pages ", false);
    unsigned int allocatedPages = 0;
    for (uint32_t i = 0; i < numPages*numDirectories; ++i)
        if (pageListArray[i].IsAllocated()) allocatedPages++;
    VGA_printf(allocatedPages, false);
    VGA_printf(" out of ", false);
    VGA_printf(maxPhysicalPages);
}

void AllocatePage(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags, bool kernel)
{
    unsigned int pageTableIndex = (virtualAddress == 0) ? 0 : (virtualAddress / pageSize);
    uint32_t* pageTable = pageTables + pageTableIndex;
    
    // Fill table then add informmation to pageListArray
    *pageTable = physicalAddress | flags;
    pageListArray[pageTableIndex] = Page(physicalAddress, true, kernel);
}

void DeallocatePage(uint32_t virtualAddress)
{
    unsigned int pageTableIndex = (virtualAddress == 0) ? 0 : (virtualAddress / pageSize);
    uint32_t* pageTable = pageTables + pageTableIndex;
    
    // Fill table then add informmation to pageListArray
    *pageTable = PD_PRESENT(0);
    pageListArray[pageTableIndex].ClearAllocated();
}

void AllocatePageDirectory(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags, bool kernel)
{
    // Find page directory to be changed - ignore divide by 0
    unsigned int pageDirectoryIndex = (virtualAddress == 0) ? 0 : (virtualAddress / pageDirectorySize);

    // Get page directory and page tables
    uint32_t* pageDirectory = &pageDirectories[pageDirectoryIndex];
    uint32_t* pageTable = pageTables + numPages*pageDirectoryIndex;

    // Fill all tables then fill directory with entry to table
    for (int i = 0; i < 1024; ++i) pageTable[i] = (i * pageSize + physicalAddress) | flags;
    *pageDirectory = (uint32_t)pageTable | flags;

    // Add information to pageListArray for all pages
    for (int i = 0; i < 1024; ++i)
    {
        pageListArray[1024*pageDirectoryIndex+i] = Page(i * pageSize + physicalAddress, true, kernel);
    }
}

void DeallocatePageDirectory(uint32_t virtualAddress)
{
    // Find page directory to be changed - ignore divide by 0
    unsigned int pageDirectoryIndex = (virtualAddress == 0) ? 0 : (virtualAddress / pageDirectorySize);

    // Clear page directory yet still tell it address for more efficient allocation of single pages
    // Whilst the page directory will be present to achieve the same end, the page tables will not
    uint32_t* pageTable = pageTables + numPages*pageDirectoryIndex;
    for (int i = 0; i < 1024; ++i) pageTable[i] = PD_PRESENT(0);
    pageDirectories[pageDirectoryIndex] = (uint32_t)pageTable | PD_PRESENT(1);
    
    // Update page list array for all pages
    for (int i = 0; i < 1024; ++i)
    {
        // Still tell it the address, as we want things at a known state before any allocation has occurred
        pageListArray[1024*pageDirectoryIndex+i] = Page(i * pageSize, false, false);
    }
}

#pragma GCC diagnostic pop