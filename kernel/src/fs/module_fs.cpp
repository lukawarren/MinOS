#include "fs/module_fs.h"

namespace fs
{
    #define VALID_FD (fd >= 0 && size_t(fd) < info.n_modules)

    ModuleFileSystem::ModuleFileSystem(const memory::MultibootInfo& _info) :
        info(_info) {}

    Optional<uint64_t> ModuleFileSystem::write(FileDescriptor, void*, uint64_t, uint64_t)
    {
        return {};
    }

    Optional<uint64_t> ModuleFileSystem::read(FileDescriptor fd, void* data, uint64_t offset, uint64_t length)
    {
        if (VALID_FD)
        {
            const auto& module = info.modules[fd];
            const auto offset_clamped = MIN(module.size, offset);
            const auto length_clamped = MIN(module.size - offset_clamped, length);
            memcpy_large(data, (void*)(module.address+offset_clamped), length_clamped);
            return length_clamped;
        }
        return {};
    }

    Optional<uint64_t> ModuleFileSystem::get_size(FileDescriptor fd)
    {
        if (VALID_FD)
        {
            const auto& module = info.modules[fd];
            return module.size;
        }
        return {};
    }

    Optional<FileDescriptor> ModuleFileSystem::get_file(const char* path)
    {
        for (size_t i = 0; i < info.n_modules; ++i) {
            if (strcmp(info.modules[i].name, path) != 0) {
                return (FileDescriptor) i;
            }
        }
        return {};
    }
}