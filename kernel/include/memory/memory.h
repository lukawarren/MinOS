#include "memory/allocator.h"
#include "memory/page_frame.h"
#include "memory/multiboot_info.h"

namespace memory
{
    constexpr size_t user_base_address = 0x40000000;
    extern PageFrame kernel_frame;
    extern Allocator allocator;

    void init(const MultibootInfo& info);

    Optional<size_t> allocate_for_user(const Optional<VirtualAddress> address, const size_t size, PageFrame& page_frame);
    Optional<size_t> allocate_for_user(const size_t size, PageFrame& page_frame);
    Optional<size_t> allocate_for_kernel(const size_t size);
    void free_for_user(const size_t address, const size_t size, PageFrame& page_frame);
}