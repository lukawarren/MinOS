#pragma once
#include "klib.h"

#define PD_PRESENT(x)               ((x & 0b1))
#define PD_READWRITE(x)             ((x & 0b1) << 1)
#define PD_GLOBALACCESS(x)          ((x & 0b1) << 2)

#define KERNEL_PAGE                 (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(0))
#define USER_PAGE                   (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(1))
#define USER_DIRECTORY              (PD_PRESENT(1) | PD_READWRITE(1) | PD_GLOBALACCESS(1))
#define USER_PAGE_READ_ONLY         (PD_PRESENT(1) | PD_READWRITE(0) | PD_GLOBALACCESS(1))
#define DISABLED_PAGE               0

#define PAGE_DIRECTORIES            1024
#define PAGE_TABLES_PER_DIRECTORY   1024
#define PAGE_SIZE                   4096
#define MAX_PAGES                   0x100000

namespace memory
{
    typedef size_t PhysicalAddress;
    typedef size_t VirtualAddress;

    class PageFrame
    {
    public:
        PageFrame(const size_t address);

        void map_page(PhysicalAddress pAddr, VirtualAddress vAddr, uint32_t flags);
        void unmap_page(VirtualAddress vAddr);
        size_t get_cr3();

        constexpr static size_t size()
        {
            return sizeof(*pageDirectories) * PAGE_DIRECTORIES +
                sizeof(*pageTables) * PAGE_TABLES_PER_DIRECTORY * PAGE_DIRECTORIES;
        }

        constexpr static bool is_page_aligned(size_t address)
        {
            return (address % PAGE_SIZE) == 0;
        }

        constexpr static size_t round_to_next_page_size(const size_t size)
        {
            const size_t remainder = size % PAGE_SIZE;
            return (remainder == 0) ? size : size + PAGE_SIZE - remainder;
        }

    private:
        size_t* pageDirectories;
        size_t* pageTables;

        void set_page_table(VirtualAddress addr, uint32_t value);
    };
}