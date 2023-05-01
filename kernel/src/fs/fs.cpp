#include "fs/fs.h"
#include "fs/module_fs.h"
#include "dev/uart.h"

namespace fs
{
    static DeviceFileSystem device_fs;
    static ModuleFileSystem module_fs;

    void init(const memory::MultibootInfo& info, void(*register_devices)(DeviceFileSystem&))
    {
        auto on_read = [](void*, uint64_t, uint64_t) { return Optional<uint64_t>{}; };
        auto on_write = [](void* data, uint64_t, uint64_t len)
        {
            for (size_t i = 0; i < len; ++i)
                uart::write_char(*((char*)data + i));
            return Optional<uint64_t>(len);
        };

        // UART devices - stdin, stdout and stderr
        device_fs.install(DeviceFile(on_read, on_write, "stdin"));
        device_fs.install(DeviceFile(on_read, on_write, "stdout"));
        device_fs.install(DeviceFile(on_read, on_write, "stderr"));

        // Other kernel devices may be registered here
        register_devices(device_fs);

        module_fs = ModuleFileSystem(info);
    }

    Optional<uint64_t> write(FileHandle handle, void* data, uint64_t offset, uint64_t length)
    {
        if(handle.file_system == 0) return device_fs.write(handle.fd, data, offset, length);
        if(handle.file_system == 1) return module_fs.write(handle.fd, data, offset, length);
        return {};
    }

    Optional<uint64_t> read(FileHandle handle, void* data, uint64_t offset, uint64_t length)
    {
        if(handle.file_system == 0) return device_fs.read(handle.fd, data, offset, length);
        if(handle.file_system == 1) return module_fs.read(handle.fd, data, offset, length);
        return {};
    }

    Optional<uint64_t> get_size(FileHandle handle)
    {
        if(handle.file_system == 0) return device_fs.get_size(handle.fd);
        if(handle.file_system == 1) return module_fs.get_size(handle.fd);
        return {};
    }

    Optional<FileHandle> get_file(const char* path)
    {
        auto result = device_fs.get_file(path);
        if (result.contains_data) return FileHandle { 0, *result };

        result = module_fs.get_file(path);
        if (result.contains_data) return FileHandle { 1, *result };

        return {};
    }
}
