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
uint32_t maxPhysicalPages;

void InitPaging(const uint32_t maxAddress)
{
    // Knowing memory size will allow for allocation in pageListArray later
    uint32_t pagingBegin = (uint32_t)(&__kernel_end); // Address from linker is aligned to nearest 4K
    uint32_t memorySize = maxAddress - pagingBegin;
    maxPhysicalPages = memorySize / pageSize;

    // Allocate enough space for all page tables and page directories, then create pointer to page list
    // uint32_t pageDirectories[numDirectories], uint32_t pageTables[numPages*numDirectories]
    pageDirectories = (uint32_t*)pagingBegin;
    pageTables = pageDirectories + numDirectories;
    pageListArray = (Page*) pageTables + numDirectories*numPages; 
    memset(pageDirectories, 0, numDirectories);
    memset(pageTables,      0, numPages);
    memset(pageListArray,   0, numDirectories*numPages);

    // Set all pages as empty and fill them with correct values 
    for (uint32_t i = 0; i < numDirectories; ++i) DeallocatePageDirectory(i * pageDirectorySize);

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
    pageListArray[pageTableIndex] = Page(0, false, false);
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
        pageListArray[1024*pageDirectoryIndex+i] = Page(0, false, false);
    }
}

void* kmalloc(uint32_t bytes)
{
    // Yes, I *know* this is very inefficient, wastteful, etc but...
    // Links lists are booooooooring

    auto RoundUpToNextPageSize = [&](uint32_t number)
    {
        int remainder = number % pageSize;
        if (remainder == 0)
            return number;
        return number + pageSize - remainder;
    };

    uint32_t pagesRequired = RoundUpToNextPageSize(bytes) / pageSize;

    // Go through each page until a group of pages are found that satisfy the size requirements
    // TODO: Optimise - if the next X pages are full, simply skip them
    for (uint32_t i = 0; i < maxPhysicalPages; ++i)
    {
        if (!pageListArray[i].IsAllocated())
        {
            bool contigousPagesFound = true;
            for (uint32_t p = 1; p < pagesRequired; ++p)
            {
                if (pageListArray[i+p].IsAllocated()) { contigousPagesFound = false; break; }
            }

            if (contigousPagesFound)
            {
                uint32_t pageAddress = pageSize*i;

                for (uint32_t p = 0; p < pagesRequired; ++p)
                    AllocatePage(pageAddress+pageSize*p, pageAddress+pageSize*p, PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1), true);

                // Clear pages too
                memset((void*)pageAddress, 0, pageSize*pagesRequired);
                
                return (void*)pageAddress;
            }
        }
    }

    VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED);
    VGA_printf("kmalloc ran out of pages!");

    return NULL;
}

void kfree(void* ptr, uint32_t bytes)
{
    auto RoundUpToNextPageSize = [&](uint32_t number)
    {
        int remainder = number % pageSize;
        if (remainder == 0)
            return number;
        return number + pageSize - remainder;
    };

    uint32_t pagesRequired = RoundUpToNextPageSize(bytes) / pageSize;

    for (uint32_t i = 0; i < pagesRequired; ++i)
        DeallocatePage((uint32_t)ptr + i*pageSize);
}

void PrintPaging()
{
    /*
        Yes, it's very inefficient,
        Yes, it's sloppy
        But yes, it's for debugging
    */
   
    auto FindContiguousChunk = [&](Page* startingPage)
    {
        Page* page = startingPage;
        int linearOffset = page->GetAddress() - ((startingPage - pageListArray) * pageSize);
        //VGA_printf(linearOffset, true, VGA_COLOUR_LIGHT_YELLOW);
        while (page->IsAllocated())
        {
            int offset = page->GetAddress() - ((page - pageListArray) * pageSize);
            //VGA_printf(offset, true, VGA_COLOUR_LIGHT_GREEN);
            if (offset != linearOffset) return --page;
            page++;
        }

        return --page;
    };
    
    VGA_printf("Linear Address", false);
    VGA_column += 26;
    VGA_printf("Virtual Address");

    Page* page = pageListArray;
    while (page < pageListArray+numDirectories*numPages)
    {
        Page* oldPage = page;
        page = FindContiguousChunk(page);
        if (page->IsAllocated())
        {
            uint32_t beforeWidth;
            const uint32_t width = 15;

            beforeWidth = VGA_column;
            VGA_printf<uint32_t, true>((oldPage - pageListArray) * pageSize, false);
            VGA_column = beforeWidth + width;
            beforeWidth = VGA_column;
            VGA_printf<uint32_t, true>((page - pageListArray) * pageSize + pageSize-1, false);
            VGA_column = beforeWidth + width;

            VGA_column += 10;

            beforeWidth = VGA_column;
            VGA_printf<uint32_t, true>(oldPage->GetAddress(), false);
            VGA_column = beforeWidth + width;
            beforeWidth = VGA_column;
            VGA_printf<uint32_t, true>(page->GetAddress() + pageSize-1);
            VGA_column=  0;
        }
        page = page+2;
    }
    
}

#pragma GCC diagnostic pop