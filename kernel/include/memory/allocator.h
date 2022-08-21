#pragma once
#include "klib.h"

namespace memory
{
    class Allocator
    {
    public:
        Allocator(const size_t address, const size_t size);
        Allocator() {}

        Optional<size_t> allocate_pages(const size_t pages);
        void free_pages(const size_t address, const size_t pages);

    private:
        uint32_t* freeGroups; // 0 = allocated, 1 = free

        void set_page_as_allocated(const size_t group, const size_t bit);
        void set_page_as_free(const size_t group, const size_t bit);

        constexpr static size_t size();
    };
}