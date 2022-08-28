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
        memory_start = PageFrame::round_to_next_page_size(memory_start);

        // Setup paging
        kernel_frame = PageFrame(memory_start, info.framebuffer_address, info.framebuffer_size, true);

        // Map in heap
        const size_t heap_address = memory_start + PageFrame::size();
        const size_t heap_size = PageFrame::round_to_next_page_size(Allocator::size());
        kernel_frame.map_pages(heap_address, heap_address, KERNEL_PAGE, heap_size / PAGE_SIZE);

        // Create heap
        allocator = Allocator(heap_address, info.memory_end - memory_start);

        // Reserve modules then map them in
        for (size_t i = 0; i < info.n_modules; ++i)
        {
            const auto pages = PageFrame::round_to_next_page_size(info.modules[i].size) / PAGE_SIZE;

            allocator.reserve_pages(
                info.modules[i].address,
                pages
            );

            kernel_frame.map_pages(
                info.modules[i].address,
                info.modules[i].address,
                KERNEL_PAGE_READ_ONLY,
                pages
            );
        }

        // Enable paging
        cpu::set_cr3(kernel_frame.get_cr3());
        cpu::enable_paging();
    }

    Optional<size_t> allocate_for_user(
        const Optional<VirtualAddress> address,
        const size_t size, PageFrame& page_frame,
        const uint32_t flags
    )
    {
        // Allocate
        auto pages = PageFrame::round_to_next_page_size(size) / PAGE_SIZE;
        auto data = allocator.allocate_pages(pages);
        if (!data.contains_data) return {};

        // Map into memory
        if (address.contains_data)
        {
            assert(*address >= memory::user_base_address);
            page_frame.map_pages(*data, *address, flags, pages);
        }
        else
            page_frame.map_pages(*data, *data, flags, pages);

        // Map into kernel too then zero out
        kernel_frame.map_pages(*data, *data, KERNEL_PAGE, pages);
        memset((void*)*data, 0, pages * PAGE_SIZE);
        return data;
    }

    Optional<size_t> allocate_for_user(const size_t size, PageFrame& page_frame, const size_t flags)
    {
        return allocate_for_user({}, size, page_frame, flags);
    }

    Optional<size_t> allocate_for_kernel(const size_t size)
    {
        auto pages = PageFrame::round_to_next_page_size(size) / PAGE_SIZE;
        auto data = allocator.allocate_pages(pages);
        if (!data) return {};

        // Zero out
        kernel_frame.map_pages(*data, *data, KERNEL_PAGE, pages);
        memset((void*)*data, 0, pages * PAGE_SIZE);
        return data;
    }

    void free_for_user(const size_t address, const size_t size, PageFrame& page_frame)
    {
        // Free from allocator...
        allocator.free_pages(address, size / PAGE_SIZE);

        // ...then unmap
        page_frame.unmap_pages(address, size / PAGE_SIZE);
    }
}