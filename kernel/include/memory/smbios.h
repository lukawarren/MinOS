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
        char entry_point_magic[5]; // must read "_DMI_"
        uint8_t checksum2;
        uint16_t structure_table_length;
        uint32_t structure_table_address;
        uint16_t n_structures;
        uint8_t bcd_revision;

        bool is_valid() const
        {
            return magic[0] == '_' && magic[1] == 'S' &&
                    magic[2] == 'M' && magic[3] == '_' &&
                    length == 0x1f;
        }
    } __attribute__((packed));

    enum StructureType : uint8_t
    {
        BIOS = 0,
        PROCESSOR_INFO = 4
    };

    struct StructureHeader
    {
        StructureType type;
        uint8_t length;
        uint16_t handle;

        size_t get_length() const;
        const char* get_nth_string(const size_t n) const;
    } __attribute__((packed));

    typedef uint8_t StringEntry;

    struct StructureBIOS
    {
        StringEntry vendor;
        StringEntry version;
        uint16_t starting_address_segment;
        StringEntry release_date;
        uint8_t rom_size;
        uint64_t characteristics;
    } __attribute__((packed));

    struct StructureProcessor
    {
        StringEntry socket_designation;
        uint8_t type;
        uint8_t family;
        StringEntry manufacturer;
        uint64_t id;
        StringEntry version;
        uint8_t voltage;
        uint16_t external_clock;
        uint16_t max_speed;
        uint16_t current_speed;
        uint8_t status;
        uint8_t upgrade;
        uint16_t l1_cache_handle;
        uint16_t l2_cache_handle;
        uint16_t l3_cache_handle;
        StringEntry serial_number;
        StringEntry asset_tag;
    } __attribute__((packed));

    void parse();
}