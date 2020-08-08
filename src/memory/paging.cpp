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
    uint32_t* pageDirectory = (uint32_t*)pagingBegin;
    for (int i = 0; i < 1024; ++i) pageDirectory[i] = PD_PRESENT(0) | PD_READWRITE(1) | PD_SUPERVISOR(1);

    /*
        Create a first page table, mapping up to 4MB of memory.
        As the address is page aligned in the loop below,
        it will always leave 12 bytes zerod - the perfect
        place for attributes ;)
    */
    uint32_t* pageTable = (uint32_t*)(pagingBegin + PAGE_SIZE);
    uint32_t mappingBegin = 0;
    for (int i = 0; i < 1024; ++i) pageTable[i] = (i * PAGE_SIZE) | (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));

    // Add our new page table to the first page directory
    pageDirectory[0] = ((unsigned int)pageTable) |  (PD_PRESENT(1) | PD_READWRITE(1) | PD_SUPERVISOR(1));
    LoadPageDirectory((uint32_t(&(pageDirectory[0]))));
    EnablePaging();
}