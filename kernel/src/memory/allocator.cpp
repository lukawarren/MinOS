#include "memory/allocator.h"

namespace memory
{
    constexpr size_t bitsPerGroup = 32;
    constexpr size_t groups = MAX_PAGES / bitsPerGroup;

    static uint32_t get_position_of_least_significant_bit(uint32_t number);

    Allocator::Allocator(const size_t address, const size_t size) : frame(address)
    {
        // Page frame created; make bitmap of free pages straight after
        freeGroups = (uint32_t*)(address + PageFrame::size());

        // Set all pages as used...
        memset(freeGroups, 0, sizeof(freeGroups[0]) * groups);

        // ...save for the ones that're free
        free_pages(address, size / PAGE_SIZE);
        println("loaded root allocator");
    }

    void* Allocator::allocate_pages(const size_t pages)
    {
        if (pages == 1)
        {
            // Search for first non-zero page group; page found!
            for (size_t i = 0; i < groups; ++i)
            {
                if (freeGroups[i] != 0)
                {
                    auto nthBit = get_position_of_least_significant_bit(freeGroups[i]);
                    auto nthPage = i * bitsPerGroup + nthBit;
                    auto address = nthPage * PAGE_SIZE;

                    set_page_as_allocated(i, nthBit);
                    return (void*)address;
                }
            }
        }
        else if (pages <= bitsPerGroup)
        {
            // We want to find a group with N 1's in a row...
            for (size_t i = 0; i < groups; ++i)
            {
                // ...which won't happen if the group's full
                if (freeGroups[i] == 0) continue;

                // Skip if there aren't enough consecutive bits (except the code
                // below won't work with a fully empty group, though thankfully
                // we can just skip that)
                auto bits = freeGroups[i];
                if (freeGroups[i] + 1 != 0)
                {
                    // Do "bits & (bits >> 1)" for 2 pages, "bits & (bits >> 1) & (bits >> 2)" for 3, etc
                    for (size_t j = 1; j < pages; ++j)
                        bits &= freeGroups[j] >> j;

                    // Any non-zero value means we've found a valid group!
                    if (bits == 0) continue;
                }

                // Valid group; the first page will be the LSB of what's returned
                // (bits go this way: 32nd bit <-------- 0th bit)
                auto nthBit = get_position_of_least_significant_bit(bits);
                auto nthPage = i * bitsPerGroup + nthBit;
                auto address = nthPage * PAGE_SIZE;

                for (size_t j = 0; j < pages; ++j)
                    set_page_as_allocated(i, nthBit + j);

                return (void*)address;
            }
        }

        else assert(false);
        return (void*)0;
    }

    void Allocator::free_pages(const size_t address, const size_t pages)
    {
        for (size_t i = 0; i < pages; ++i)
        {
            const auto page = address / PAGE_SIZE + i;
            set_page_as_free(page / bitsPerGroup, page % bitsPerGroup);
        }
    }

    void Allocator::set_page_as_allocated(const size_t group, const size_t bit)
    {
        freeGroups[group] &= ~(1 << bit);
    }

    void Allocator::set_page_as_free(const size_t group, const size_t bit)
    {
        freeGroups[group] |= (1 << bit);
    }

    /*
        https://graphics.stanford.edu/~seander/bithacks.html
        "Count the consecutive zero bits (trailing) on the right with multiply and lookup"
        The expression (v & -v) extracts the least significant 1 bit from v. The constant 0x077CB531UL is a de Bruijn sequence,
        which produces a unique pattern of bits into the high 5 bits for each possible bit position that it is multiplied against.
        When there are no bits set, it returns 0.
    */
    static uint32_t get_position_of_least_significant_bit(const uint32_t number)
    {
        static const uint32_t multiplyDeBruijnBitPosition[32] =
        {
            0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
            31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
        };

        return multiplyDeBruijnBitPosition[((uint32_t)((number & -number) * 0x077CB531U)) >> 27];
    }
}