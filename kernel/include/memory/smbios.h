#pragma once
#include "klib.h"

namespace memory::smbios
{
    // SMBIOS 2 entry; version 3 is for 64-bit systems
    struct EntryPoint
    {
        char magic[4]; // must read "_SM_"
        uint8_t checksum;
        uint8_t length; // SMBIOS 2.1 should have this at 0x1f
        uint8_t major_version;
        uint8_t minor_version;
        uint16_t max_structure_size;
        uint8_t entry_point_revision;
        char formatted_area[5];
        char entry_point_magic[2]; // must read "_DMI_"
        uint8_t checksum2;
        uint16_t table_length;
        uint32_t table_address;
        uint16_t n_structures;
        uint8_t bcd_revision;

        bool is_valid() const
        {
            return magic[0] == '_' && magic[1] == 'S' &&
                    magic[2] == 'M' && magic[3] == '_' &&
                    length == 0x1f;
        }
    };

    void parse();
}