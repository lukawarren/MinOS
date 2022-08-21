#pragma once
#include "klib.h"

namespace fs
{
    class DeviceFile
    {
    public:
        DeviceFile(void (*on_read)(void*, size_t), size_t (*on_write)(void*, size_t))
        {
            this->on_read = on_read;
            this->on_write = on_write;
        }

        DeviceFile() {}
        ~DeviceFile() {}

        void read(void* data, size_t len)
        {
            on_read(data, len);
        }

        size_t write(void* data, size_t len)
        {
            return on_write(data, len);
        }

        void (*on_read)(void*, size_t);
        size_t (*on_write)(void*, size_t);
    };
}