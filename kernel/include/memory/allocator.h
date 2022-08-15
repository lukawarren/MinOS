#pragma once
#include "klib.h"
#include "memory/pageFrame.h"

namespace memory
{
    class Allocator
    {
    public:
        Allocator(const size_t address, const size_t size);

        void* get_user_memory_at_address(const VirtualAddress address, const size_t size);

        size_t get_cr3() { return frame.get_cr3(); }

    private:
        PageFrame frame;
        uint32_t* freeGroups; // 0 = allocated, 1 = free

        void* allocate_pages(const size_t pages);
        void free_pages(const size_t address, const size_t pages);

        void set_page_as_allocated(const size_t group, const size_t bit);
        void set_page_as_free(const size_t group, const size_t bit);
    };
}