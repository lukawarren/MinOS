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

    /*
        Create a first page table, mapping up to 4MB of memory.
        As the address is page aligned in the loop below,
        it will always leave 12 bytes zerod - the perfect
        place for attributes. This page will be identity mapped
        (1:1), and will house all the important kernel data.
    */
    volatile uint32_t* identityMappedKernel = (uint32_t*)(pagingBegin + PAGE_SIZE);
    for (int i = 0; i < 1024; ++i) identityMappedKernel[i] = (i * PAGE_SIZE) | (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));
    pageDirectory[0] = ((unsigned int)identityMappedKernel) |  (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));

    /* 
        Allocate next 4MB for user space, yet in such a way that
        user space begins at 16MB (0x1000000), leaving space in the future
        for a larger kernel, and perhaps memory mapped peripherals.
    */
    volatile uint32_t* identityMappedUserspace = (uint32_t*)(pagingBegin + PAGE_SIZE + PAGE_SIZE);
    for (int i = 0; i < 1024; ++i) identityMappedUserspace[i] = (0x1000000 + i * PAGE_SIZE) | (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));
    pageDirectory[1] = ((unsigned int)identityMappedUserspace) |  (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));

    /*
        Load page directory - as our kernel is identity mapped and lies
        at the start of memory (or 1MB) anyway, nothing needs to change
        as far as the kernel is concerned.
    */
    LoadPageDirectory((uint32_t)&(pageDirectory[0]));
    EnablePaging();
}