#include "fs/fs.h"
#include "io/uart.h"

namespace fs
{
    DeviceFile files[3];
    constexpr size_t stdout = 1;
    constexpr size_t stderr = 2;

    void init()
    {
        auto on_read = [](void*, size_t) {};

        auto on_write = [](void* data, size_t len)
        {
            for (size_t i = 0; i < len; ++i)
                uart::write_char(*((char*)data + i));
            return len;
        };

        // Setup stdout and stderr
        files[stdout] = DeviceFile(on_read, on_write);
        files[stderr] = DeviceFile(on_read, on_write);
    }

    Optional<DeviceFile*> get_file(const descriptor fd)
    {
        if (fd != stdout && fd != stderr)
        {
            println("unknown fd ", fd);
            return {};
        }
        return &files[fd];
    }
}