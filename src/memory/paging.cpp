#include "paging.h"
#include "mmu.h"
#include "../io/vga.h"

void InitPaging(uint32_t maxAddress)
{
    // Address from linker is aligned to nearest 4K
    uint32_t pagingBegin = (uint32_t)(&__kernel_end);
    uint32_t memorySize = maxAddress - pagingBegin;
    uint32_t numPages = memorySize / PAGE_SIZE;

    /*
        Allocate one entire page for the page directory - 1024 entries,
        each 4 bytes large.
    */
    volatile uint32_t* pageDirectory = (uint32_t*)pagingBegin;
    for (int i = 0; i < 1024; ++i) pageDirectory[i] = PD_PRESENT(0) | PD_READWRITE(1) | PD_SUPERVISOR(1);
    pagingBegin += PAGE_SIZE;

    /*
        Create a first page table, mapping up to 4MB of memory.
        As the address is page aligned in the loop below,
        it will always leave 12 bytes zerod - the perfect
        place for attributes. This page will be identity mapped
        (1:1), and will house all the important kernel data.
    */
    volatile uint32_t* identityMappedKernel = (uint32_t*)(pagingBegin);
    for (int i = 0; i < 1024; ++i) identityMappedKernel[i] = (i * PAGE_SIZE) | (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));
    pageDirectory[0] = ((unsigned int)identityMappedKernel) |  (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));
    pagingBegin += PAGE_SIZE;

    /* 
        Allocate next 4MB for user space, yet in such a way that
        user space begins at 16MB (0x1000000), leaving space in the future
        for a larger kernel, and perhaps memory mapped peripherals.
    */
    volatile uint32_t* identityMappedUserspace = (uint32_t*)(pagingBegin);
    for (int i = 0; i < 1024; ++i) identityMappedUserspace[i] = (0x1000000 + i * PAGE_SIZE) | (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));
    pageDirectory[1] = ((unsigned int)identityMappedUserspace) |  (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));
    pagingBegin += PAGE_SIZE;

    /*
        Load page directory - as our kernel is identity mapped and lies
        at the start of memory (or 1MB) anyway, nothing needs to change
        as far as the kernel is concerned.
    */
    LoadPageDirectory((uint32_t)&(pageDirectory[0]));
    EnablePaging();

    /*
        Create an array of pages, keeping track of virtual
        addresses and other information such as if the page
        is used. Such a structure will exist after all
        the page descriptors and page tables. For now the
        maximum amount of pages is capped to allow the
        structure to fit within the remaining memory of the
        kernel's 4 MB page.
    */
    Page* pageListArray = (Page*)(pagingBegin);
    if (sizeof(Page) * numPages > 1024 * 1024 * 4) // If too much memory to fit, panic!
    {
        // (yes this is because I am lazy)
        VGA_printf("[Failure] ", false, VGA_COLOUR_LIGHT_RED);
        VGA_printf(" Too much memory to fit within paging limits!");
    }
    pagingBegin += sizeof(Page) * numPages;

    // Kernel pages
    for (int i = 0; i < 0x400000 / PAGE_SIZE; ++i) pageListArray[i] = Page(i * PAGE_SIZE, i  * PAGE_SIZE < pagingBegin, true);

    // User pages
    for (int i = 0x400000 / PAGE_SIZE; i < 0x800000 / PAGE_SIZE; ++i) pageListArray[i] = Page(i * PAGE_SIZE, false, false);

    VGA_printf("Allocated ", false);
    VGA_printf<uint32_t, false>(0x400000 / PAGE_SIZE, false);
    VGA_printf(" pages for kernel space and ", false);
    VGA_printf<uint32_t, false>(0x400000 / PAGE_SIZE, false);
    VGA_printf(" for user space");

    for (int i = 0; i < 0x400000 / PAGE_SIZE; ++i)
    {
        if (pageListArray[i].IsAllocated() == false && pageListArray[i].IsKernel())
        {
            VGA_printf("First free kernel page found at ", false);
            VGA_printf<uint32_t, true>(pageListArray[i].GetAddress());
            break;
        }
    }
}