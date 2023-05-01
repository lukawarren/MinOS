#include "memory/allocator.h"
#include "memory/page_frame.h"
#include "memory/multiboot_info.h"

namespace memory
{
    extern PageFrame kernel_frame;
    extern Allocator allocator;

    void init(const MultibootInfo& info);

    Optional<AddressPair> allocate_for_user(const Optional<VirtualAddress> address, const size_t size, PageFrame& page_frame, const size_t flags = USER_PAGE);
    Optional<AddressPair> allocate_for_user(const size_t size, PageFrame& page_frame, const size_t flags = USER_PAGE);
    Optional<size_t> allocate_for_kernel(const size_t size);
    void free_for_user(const size_t address, const size_t size, PageFrame& page_frame);
}