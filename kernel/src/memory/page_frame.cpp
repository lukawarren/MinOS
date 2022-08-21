#include "memory/page_frame.h"
#include "cpu/cpu.h"

extern size_t kernel_end;

namespace memory
{
    PageFrame::PageFrame(const size_t address)
    {
        // Store first all the page directories, followed by all the page tables...
        pageDirectories = (size_t*)address;
        pageTables = pageDirectories + PAGE_DIRECTORIES;
        assert(is_page_aligned(address));
        assert((size_t)&kernel_end);

        // ...taking care to clear out memory first
        memset((void*)address, 0, size());

        // Populate page directories
        for (size_t i = 0; i < PAGE_DIRECTORIES; ++i)
        {
            const auto flags = USER_DIRECTORY;
            const auto tables = (size_t)&pageTables[i * PAGE_TABLES_PER_DIRECTORY];
            pageDirectories[i] = tables | flags;
        }

        // Map kernel code (regardless of ring) so we can still service IRQs, etc.
        const size_t mapped_pages = ((size_t)&kernel_end) / PAGE_SIZE;
        map_pages(0, 0, KERNEL_PAGE, mapped_pages);

        // Map in ourselves!
        map_pages(address, address, KERNEL_PAGE, size() / PAGE_SIZE);

        // Unmap first page so null pointers crash us (flushes TLB too!)
        unmap_page(0);
        println("created page frame");
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

    void PageFrame::map_pages(PhysicalAddress pAddr, VirtualAddress vAddr, uint32_t flags, size_t pages)
    {
        for (size_t i = 0; i < pages; ++i)
            map_page(pAddr + i*PAGE_SIZE, vAddr + i*PAGE_SIZE, flags);
    }

    void PageFrame::unmap_page(VirtualAddress vAddr)
    {
        // Revert back to disabled
        assert(is_page_aligned(vAddr));
        set_page_table(vAddr, DISABLED_PAGE);
        cpu::flush_tlb();
    }

    void PageFrame::unmap_pages(VirtualAddress vAddr, size_t pages)
    {
        for (size_t i = 0; i < pages; ++i)
            unmap_page(vAddr + i*PAGE_SIZE);
    }

    size_t PageFrame::virtual_address_to_physical(VirtualAddress vAddr) const
    {
        // Read entry from page table
        const size_t offset = vAddr % PAGE_SIZE;
        size_t table = pageTables[vAddr == 0 ? 0 : vAddr / PAGE_SIZE];
        return (table & 0xfffff000) + offset;
    }

    size_t PageFrame::get_cr3() const
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