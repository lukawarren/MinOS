#include "memory/smbios.h"

namespace memory::smbios
{
    static Optional<EntryPoint*> get_entry_point_address();
    static void print_info(const StructureHeader* header);

    void parse()
    {
        // Find entry point
        auto ptr = get_entry_point_address();
        if (!ptr) { assert(false); return; }
        auto* entry = *ptr;

        // Iterate over structures
        if (entry->n_structures == 0) return;
        auto* header = (StructureHeader*)entry->structure_table_address;

        for (auto i = 0; i < entry->n_structures; ++i)
        {
            print_info(header);
            header = (StructureHeader*)((size_t)header + header->get_length());
        }
    }

    static Optional<EntryPoint*> get_entry_point_address()
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

    static void print_info(const StructureHeader* header)
    {
        if (header->type == BIOS)
        {
            auto* data = (StructureBIOS*)(header+1);
            println("BIOS vendor = ", header->get_nth_string(data->vendor));
            println("BIOS version = ", header->get_nth_string(data->version));
            println("BIOS release date = ", header->get_nth_string(data->release_date));
        }

        if (header->type == PROCESSOR_INFO)
        {
            auto* data = (StructureProcessor*)(header+1);
            println("CPU manufacturer = ", header->get_nth_string(data->manufacturer));
            println("CPU socket = ", header->get_nth_string(data->socket_designation));
        }
    }

    size_t StructureHeader::get_length() const
    {
        /*
            The length field includes the length of the header and the structure's
            data itself, but does *not* include the length of the string-set beneath
            which is terminated by two consecutive null bytes.
        */
        size_t i;
        char* string_set = (char*)this + length;
        for (i = 1; string_set[i-1] != '\0' || string_set[i] != '\0'; ++i);
        return length + i + 1;
    }

    const char* StructureHeader::get_nth_string(const size_t n) const
    {
        /*
            String 1 is the 1st string, string 2 is the 2nd string, etc.
            Each is null terminated.
        */
        size_t i = 0;
        char* string_set = (char*)this + length;

        for (size_t n1 = 1; n1 < n; ++n1)
        {
            // Skip past string by finding null terminator
            for (; string_set[i] != '\0'; ++i);
            ++i;
        }

        return string_set + i;
    }
}