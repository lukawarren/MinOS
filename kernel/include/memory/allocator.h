#pragma once
#include "klib.h"
#include "memory/page_frame.h"

namespace memory
{
    constexpr size_t bits_per_group = 32;
    constexpr size_t groups = MAX_PAGES / bits_per_group;

    class Allocator
    {
    public:
        Allocator(const size_t address, const size_t size);
        Allocator() {}

        Optional<size_t> allocate_pages(const size_t pages);
        void reserve_pages(const size_t address, const size_t pages);
        void free_pages(const size_t address, const size_t pages);

        constexpr static size_t size()
        {
            return sizeof(freeGroups[0]) * groups;
        }

    private:
        uint32_t* freeGroups; // 0 = allocated, 1 = free

        void set_page_as_allocated(const size_t group, const size_t bit);
        void set_page_as_free(const size_t group, const size_t bit);
    };
}