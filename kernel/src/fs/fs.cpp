#include "fs/fs.h"
#include "io/uart.h"

namespace fs
{
    DeviceFile files[6];
    constexpr size_t stdout = 1;
    constexpr size_t stderr = 2;
    constexpr size_t keyboard = 3;
    constexpr size_t doom_wad = 4;

    // Temporary! Remember to remove from fs.h too :)
    char keyboard_buffer[256] = {};
    size_t keyboard_buffer_index = 0;

    // Temporary! DOOM wad file
    size_t wad_address;
    size_t wad_size;
    size_t wad_seek;

    void init(const memory::MultibootInfo& info)
    {
        auto on_read = [](void*, size_t) { return (size_t)0; };

        auto on_write = [](void* data, size_t len)
        {
            for (size_t i = 0; i < len; ++i)
                uart::write_char(*((char*)data + i));
            return len;
        };

        // Setup stdout and stderr
        files[stdout] = DeviceFile(on_read, on_write);
        files[stderr] = DeviceFile(on_read, on_write);

        // Very basic keyboard input
        files[keyboard] = DeviceFile([](void* data, size_t len){
            size_t r_len = MIN(len, keyboard_buffer_index);
            memcpy(data, keyboard_buffer, r_len);
            keyboard_buffer_index -= r_len;
            return r_len;
        }, [](void*, size_t){ return (size_t) 0; });

        // DOOM wad hack
        wad_address = info.modules[1].address;
        wad_size = info.modules[1].size;
        wad_seek = 0;
        files[doom_wad] = DeviceFile([](void* data, size_t len){
            size_t size = MIN(len, wad_size-wad_seek);
            memcpy(data, (void*)(wad_address+wad_seek), size);
            return size;
        }, [](void*, size_t){ return (size_t) 0; });
    }

    Optional<DeviceFile*> get_file(const descriptor fd)
    {
        if (fd != stdout && fd != stderr && fd != keyboard && fd != doom_wad)
        {
            println("unknown fd ", fd);
            return {};
        }
        return &files[fd];
    }
}
