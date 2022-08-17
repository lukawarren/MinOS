#include "memory/memory.h"
#include "cpu/cpu.h"

extern size_t kernel_end;

namespace memory
{
    PageFrame kernel_frame;
    Allocator allocator;

    void init(const MultibootInfo& info)
    {
        // Figure out where to put memory - preferably well after the kernel
        // and all the multiboot stuff
        size_t memory_start = MAX(info.memory_begin, (size_t) &kernel_end);
        memory_start = MAX(memory_start, info.get_highest_module_address());
        memory_start = memory::PageFrame::round_to_next_page_size(memory_start);

        // Setup paging, heap, etc.
        kernel_frame = PageFrame(memory_start, false);
        allocator = Allocator(memory_start + kernel_frame.size(), info.memory_end - memory_start);
        cpu::set_cr3(kernel_frame.get_cr3());
        cpu::enable_paging();
    }

    void* allocate_for_user(const VirtualAddress address, const size_t size, PageFrame& page_frame)
    {
        // Allocate
        auto pages = PageFrame::round_to_next_page_size(size) / PAGE_SIZE;
        void* data = allocator.allocate_pages(pages);
        assert(PageFrame::is_page_aligned((size_t)data));

        // Map into memory
        for (size_t i = 0; i < pages; ++i)
        {
            size_t offset = PAGE_SIZE*i;
            page_frame.map_page((size_t)data + offset, address + offset, USER_PAGE);
        }

        return data;
    }

    void* allocate_for_kernel(const size_t size)
    {
        auto pages = PageFrame::round_to_next_page_size(size) / PAGE_SIZE;
        void* data = allocator.allocate_pages(pages);
        assert(PageFrame::is_page_aligned((size_t)data));
        return data;
    }
}