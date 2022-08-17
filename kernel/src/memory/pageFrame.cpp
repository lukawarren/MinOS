#include "memory/pageFrame.h"
#include "cpu/cpu.h"

extern size_t kernel_end;

namespace memory
{
    PageFrame::PageFrame(const size_t address, const bool is_userspace)
    {
        // Store first all the page directories, followed by all the page tables
        pageDirectories = (size_t*)address;
        pageTables = pageDirectories + PAGE_DIRECTORIES;
        assert(is_page_aligned(address));

        // Populate page directories
        for (unsigned int i = 0; i < PAGE_DIRECTORIES; ++i)
        {
            const auto flags = USER_DIRECTORY;
            const auto tables = (size_t)&pageTables[i * PAGE_TABLES_PER_DIRECTORY];
            pageDirectories[i] = tables | flags;
        }

        // Kernel: identify map all of memory
        // Userspace: map only kernel code so we can still service IRQs, etc.
        const size_t mapped_pages = is_userspace ?
            ((size_t)&kernel_end) / PAGE_SIZE :
            PAGE_TABLES_PER_DIRECTORY * PAGE_DIRECTORIES;

        for (unsigned int i = 0; i < mapped_pages; ++i)
        {
            const auto flags = KERNEL_PAGE;
            const PhysicalAddress p_addr = i * PAGE_SIZE;
            pageTables[i] = p_addr | flags;
        }

        // Unmap first page so null pointers crash us (flushes TLB too!)
        unmap_page(0);
    }

    void PageFrame::map_page(PhysicalAddress pAddr, VirtualAddress vAddr, uint32_t flags)
    {
        assert(is_page_aligned(pAddr));
        assert(is_page_aligned(vAddr));

        // The physical location of the page table / directory represents the virtual address mapped,
        // and the address inside a page table itself represents the physical address mapped.
        // i.e. the virtual address is derived from the physical address of the entry,
        //      and the physical address is derived from the entry's data
        set_page_table(vAddr, pAddr | flags);
        cpu::flush_tlb();
    }

    void PageFrame::unmap_page(VirtualAddress vAddr)
    {
        // Revert back to disabled
        assert(is_page_aligned(vAddr));
        set_page_table(vAddr, DISABLED_PAGE);
        cpu::flush_tlb();
    }

    size_t PageFrame::get_cr3()
    {
        return (size_t) &pageDirectories[0];
    }

    void PageFrame::set_page_table(VirtualAddress addr, uint32_t value)
    {
        // If we're setting it, check it's unset first to avoid "collisions"
        size_t* table = &pageTables[addr == 0 ? 0 : addr / PAGE_SIZE];
        assert(value == 0 || *table == DISABLED_PAGE);
        *table = value;
    }
}