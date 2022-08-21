#include "memory/allocator.h"
#include "memory/pageFrame.h"

namespace memory
{
    constexpr size_t bits_per_group = 32;
    constexpr size_t groups = MAX_PAGES / bits_per_group;

    static uint32_t get_position_of_least_significant_bit(uint32_t number);

    Allocator::Allocator(const size_t address, const size_t size)
    {
        // Set all pages as used...
        freeGroups = (uint32_t*)address;
        memset(freeGroups, 0, Allocator::size());

        // ...save for the ones that're free
        size_t structures_end = (size_t) freeGroups + Allocator::size();
        free_pages(structures_end, size / PAGE_SIZE);
        println("created root allocator");
    }

    Optional<size_t> Allocator::allocate_pages(const size_t pages)
    {
        if (pages == 1)
        {
            // Search for first non-zero page group; page found!
            for (size_t i = 0; i < groups; ++i)
            {
                if (freeGroups[i] != 0)
                {
                    auto nth_bit = get_position_of_least_significant_bit(freeGroups[i]);
                    auto nth_page = i * bits_per_group + nth_bit;
                    auto address = nth_page * PAGE_SIZE;

                    set_page_as_allocated(i, nth_bit);
                    return address;
                }
            }
        }
        else if (pages <= bits_per_group)
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
                auto nth_bit = get_position_of_least_significant_bit(bits);
                auto nth_page = i * bits_per_group + nth_bit;
                auto address = nth_page * PAGE_SIZE;

                for (size_t j = 0; j < pages; ++j)
                    set_page_as_allocated(i, nth_bit + j);

                return address;
            }
        }
        else
        {
            // Keep a running total of pages found. If a new group lets all its
            // precursors down, we know anything before it's not gonna work either
            // so, let's just march forward until we find a nice sequential block!
            // NOTE: this code is approximate, and can't start mid-group, but large
            // contiguous allocations like this should hopefully be pretty infrequent.

            size_t remaining_pages = pages;
            size_t starting_group = 0;

            for (size_t i = 0; i < groups; ++i)
            {
                const size_t bitmap = freeGroups[i];

                // If we're not the last group
                if (remaining_pages > bits_per_group)
                {
                    // If the group's not all free, give up
                    if (freeGroups[i] + 1 != 0)
                    {
                        remaining_pages = pages;
                        starting_group = i + 1;
                    }

                    else remaining_pages -= bits_per_group;
                }
                else
                {
                    // We're in the last group, so our remaining pages need to be
                    // at the *start* of the group
                    const size_t mask = (1 << remaining_pages) - 1;

                    if ((bitmap & mask) == mask)
                    {
                        // Success - map and return away!
                        const size_t nth_page = starting_group * bits_per_group;

                        for (size_t page = 0; page < pages; ++page)
                        {
                            const size_t group = (nth_page + page) / bits_per_group;
                            const size_t bit = (nth_page + page) % bits_per_group;
                            set_page_as_allocated(group, bit);
                        }

                        return nth_page * PAGE_SIZE;
                    }

                    // There weren't enough free pages at the start, and
                    // we know this one's not full, so give up and skip ahead
                    remaining_pages = pages;
                    starting_group = i;
                }
            }
        }

        return {};
    }

    void Allocator::free_pages(const size_t address, const size_t pages)
    {
        assert(PageFrame::is_page_aligned(address));

        for (size_t i = 0; i < pages; ++i)
        {
            const auto page = address / PAGE_SIZE + i;
            set_page_as_free(page / bits_per_group, page % bits_per_group);
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

    constexpr size_t Allocator::size()
    {
        return sizeof(freeGroups[0]) * groups;
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
        static const uint32_t multiply_de_bruijn_bit_position[32] =
        {
            0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
            31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
        };

        return multiply_de_bruijn_bit_position[((uint32_t)((number & -number) * 0x077CB531U)) >> 27];
    }
}