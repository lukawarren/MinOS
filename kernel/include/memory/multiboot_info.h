#pragma once
#include "multiboot.h"
#include "klib.h"

#define MAX_MODULES 32

namespace memory
{
    class MultibootInfo
    {
    public:
        MultibootInfo(const multiboot_info_t* info);
        size_t get_highest_module_address() const;

        struct Module
        {
            char name[64];
            size_t address;
            size_t size;

            Module() {}

            Module(size_t start, size_t end, char* name)
            {
                address = start;
                size = end - start;
                strncpy(this->name, name, sizeof(this->name) / sizeof(this->name[0]));
            }
        };

        Module modules[MAX_MODULES];
        size_t n_modules = 0;

        size_t memory_begin = 0;
        size_t memory_end = 0;

    private:

        void parse_modules(const multiboot_info_t* info);
        void parse_memory(const multiboot_info_t* info);
    };
}