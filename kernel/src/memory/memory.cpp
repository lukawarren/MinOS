#include "memory/memory.h"
#include "stdout/uart.h"
#include "cpu/cpu.h"
#include "stdlib.h"

extern uint32_t __kernel_end;

// Page directories and tables
static uint32_t* pageDirectories;
static uint32_t* pageTables;

namespace Memory
{
    void Init(const multiboot_info_t* pMultiboot)
    {
        // Get size of memory
        const uint32_t upperBound = GetMaxMemory(pMultiboot);
        uint32_t* pFreeMemory = &__kernel_end; // Linker has aligned to nearest 4k
        UART::WriteString("Allocating memory of size ");
        UART::WriteNumber(upperBound);
        UART::WriteString("\n");

        // Allocate space for page tables and directories
        pageDirectories = pFreeMemory;
        pageTables = (uint32_t*)((uint32_t)pageDirectories + NUM_DIRECTORIES*4); // To get rid of warning

        // Clear pages
        for (uint32_t i = 0; i < NUM_DIRECTORIES; ++i) DeallocatePageDirectory(i * DIRECTORY_SIZE, USER_PAGE);

        // Identity-map kernel pages
        const uint32_t userspaceBegin = (uint32_t)(pageTables + NUM_PAGES); // Should be aligned to nearest 4k anyway
        for (uint32_t i = 0; i < userspaceBegin / PAGE_SIZE; ++i)
        {
            AllocatePage(i * PAGE_SIZE, i * PAGE_SIZE, KERNEL_PAGE);
        }

        // Enable paging
        CPU::LoadPageDirectories((uint32_t)pageDirectories);
        CPU::EnablePaging();
    }

    uint32_t GetMaxMemory(const multiboot_info_t* pMultiboot)
    {
        /*
            Attempt to find a "reasonable" contiguous region of memory.
            To that end, enumerate the multiboot's memory map and settle
            on the 2nd (ish) chunck, starting from 0x1000000 (16mb) and
            extending to just before the memory mapped PCI devices (if any)
        */

        multiboot_memory_map_t* pMemoryMap = (multiboot_memory_map_t*)pMultiboot->mmap_addr;
       
        while ((uint32_t)pMemoryMap < pMultiboot->mmap_addr + pMultiboot->mmap_length)
        {
            if (pMemoryMap->addr == 0x100000)
            {
                return (uint32_t)(pMemoryMap->addr + pMemoryMap->len);
            }
            pMemoryMap = (multiboot_memory_map_t*) ((uint32_t)pMemoryMap + pMemoryMap->size + sizeof(pMemoryMap->size));
        }

        return 0;
    }

    void DeallocatePageDirectory(const uint32_t virtualAddress, const uint32_t flags)
    {
        // Find page directory to be changed - ignore divide by 0
        unsigned int pageDirectoryIndex = (virtualAddress == 0) ? 0 : (virtualAddress / DIRECTORY_SIZE);

        // Pages can be allocated easier if we leave the page directory
        // allocated but deallocate all page tables
        assert((flags & 0b1) == PD_PRESENT(1));

        // Set page tables
        uint32_t* pageTable = pageTables + NUM_PAGES*pageDirectoryIndex;
        for (int i = 0; i < NUM_PAGES; ++i) pageTable[i] = PD_PRESENT(0);

        // Set page directory
        pageDirectories[pageDirectoryIndex] = (uint32_t)pageTable | flags;

        CPU::FlushTLB();
    }

    void AllocatePage(uint32_t physicalAddress, uint32_t virtualAddress, uint32_t flags)
    {
        // Find page table - ignore divide by 0
        unsigned int pageTableIndex = (virtualAddress == 0) ? 0 : (virtualAddress / PAGE_SIZE);
        uint32_t* pageTable = pageTables + pageTableIndex;

        // Fill table and flush TLB
        *pageTable = physicalAddress | flags;
        CPU::FlushTLB();
    }
}