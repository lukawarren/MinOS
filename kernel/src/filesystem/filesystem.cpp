#include "filesystem/filesystem.h"
#include "filesystem/file.h"
#include "filesystem/cpio.h"
#include "memory/modules.h"
#include "io/uart.h"

namespace Filesystem
{
    static File pFiles[FileDescriptors::N_FILES];

    void Init()
    {
        // Load CPIO file
        uint32_t pFile = Modules::GetModule();
        CPIO::sHeader* pHeader = (CPIO::sHeader*)pFile;

        // Check magic
        assert(pHeader->ConformsToMagic());
        assert(pHeader->IsFile());

        // Get name, name size and data
        const char* sName = (const char*) (pHeader+1);
        const uint32_t nameSize = pHeader->StringToDecimal(pHeader->nameSize, 6);
        void* pData = (void*)(pFile + sizeof(CPIO::sHeader) + nameSize);

        // Get size of data
        const uint32_t fileSize = pHeader->StringToDecimal(pHeader->fileSize, 11);

        pFiles[FileDescriptors::userspace] = File(fileSize, pData);
    }

    File* GetFile(const FileDescriptor fd)
    {
        assert(fd < FileDescriptors::N_FILES);
        return &pFiles[fd];
    }

}