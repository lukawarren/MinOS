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

        // Setup paging
        kernel_frame = PageFrame(memory_start);

        // Map in heap
        const size_t heap_address = memory_start + PageFrame::size();
        for (size_t i = 0; i < heap_address / PAGE_SIZE; ++i)
        {
            kernel_frame.map_page(
                heap_address + i*PAGE_SIZE,
                heap_address + i*PAGE_SIZE,
                KERNEL_PAGE
            );
        }

        // Create heap
        allocator = Allocator(memory_start + PageFrame::size(), info.memory_end - memory_start);

        // Reserve modules then map them in
        for (size_t i = 0; i < info.n_modules; ++i)
        {
            allocator.reserve_pages(
                info.modules[i].address,
                info.modules[i].size / PAGE_SIZE
            );
            kernel_frame.map_pages(
                info.modules[i].address,
                info.modules[i].address,
                KERNEL_PAGE_READ_ONLY,
                info.modules[i].size / PAGE_SIZE
            );
        }

        // Enable paging
        cpu::set_cr3(kernel_frame.get_cr3());
        cpu::enable_paging();
    }

    Optional<size_t> allocate_for_user(const Optional<VirtualAddress> address, const size_t size, PageFrame& page_frame)
    {
        // Allocate
        auto pages = PageFrame::round_to_next_page_size(size) / PAGE_SIZE;
        auto data = allocator.allocate_pages(pages);
        if (!data.contains_data) return {};

        // Map into memory
        for (size_t i = 0; i < pages; ++i)
        {
            size_t offset = PAGE_SIZE*i;

            if (address.contains_data)
            {
                assert(*address >= memory::user_base_address);
                page_frame.map_page(*data + offset, *address + offset, USER_PAGE);
            }
            else
                page_frame.map_page(*data + offset, *data + offset, USER_PAGE);
        }

        // Zero out
        memset((void*)*data, 0, pages * PAGE_SIZE);

        return data;
    }

    Optional<size_t> allocate_for_user(const size_t size, PageFrame& page_frame)
    {
        return allocate_for_user({}, size, page_frame);
    }

    Optional<size_t> allocate_for_kernel(const size_t size)
    {
        auto pages = PageFrame::round_to_next_page_size(size) / PAGE_SIZE;
        auto data = allocator.allocate_pages(pages);
        if (!data) return {};

        // Zero out
        memset((void*)*data, 0, pages * PAGE_SIZE);
        return data;
    }
}