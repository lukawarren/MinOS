#pragma once
#include "klib.h"

namespace fs
{
    class DeviceFile
    {
    public:
        DeviceFile(size_t (*on_read)(void*, size_t), size_t (*on_write)(void*, size_t))
        {
            this->on_read = on_read;
            this->on_write = on_write;
        }

        DeviceFile() {}
        ~DeviceFile() {}

        size_t read(void* data, size_t len)
        {
            return on_read(data, len);
        }

        size_t write(void* data, size_t len)
        {
            return on_write(data, len);
        }

        size_t (*on_read)(void*, size_t);
        size_t (*on_write)(void*, size_t);
    };
}