#include "memory/allocator.h"
#include "memory/pageFrame.h"
#include "memory/multiboot_info.h"

namespace memory
{
    extern PageFrame kernel_frame;
    extern Allocator allocator;

    void init(const MultibootInfo& info);

    void* allocate_for_user(const Optional<VirtualAddress> address, const size_t size, PageFrame& page_frame);
    void* allocate_for_user(const size_t size, PageFrame& page_frame);
    void* allocate_for_kernel(const size_t size);
}