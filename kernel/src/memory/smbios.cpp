#include "memory/smbios.h"

namespace memory::smbios
{
    Optional<EntryPoint*> get_entry_point_address();

    void parse()
    {
        auto ptr = get_entry_point_address();
        if (!ptr) { assert(false); return; }
        auto* entry = *ptr;
        println("major version = ", entry->major_version);
    }

    Optional<EntryPoint*> get_entry_point_address()
    {
        // The SMBIOS entry point exists anywhere from 0xf0000 - 0xfffff, on a 16-byte boundary
        auto* magic = (EntryPoint*)0xf000;
        for (;;)
        {
            if (magic->is_valid()) return magic;
            if ((size_t)magic > 0xfffff) return {};
            magic = (EntryPoint*)((size_t)magic + 16);
        }
        return {};
    }
}