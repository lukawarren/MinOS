#include "filesystem/filesystem.h"
#include "filesystem/file.h"
#include "filesystem/cpio.h"
#include "memory/modules.h"
#include "io/uart.h"

namespace Filesystem
{
    static File pFiles[FileDescriptors::MAX_FILES];

    void Init()
    {
        // Load CPIO file
        uint32_t pFile = Modules::GetModule();
        CPIO::sHeader* pHeader = (CPIO::sHeader*)pFile;

        int count = 1;
        while (pHeader->IsTerminator() == false)
        {
            // Check magic
            assert(pHeader->ConformsToMagic());
            assert(pHeader->IsFile());

            // Get name size and data
            const uint32_t nameSize = pHeader->StringToDecimal(pHeader->nameSize, 6);
            void* pData = (void*)(pFile + sizeof(CPIO::sHeader) + nameSize);

            // Get size of data
            const uint32_t fileSize = pHeader->StringToDecimal(pHeader->fileSize, 11);

            // Allocate data so it's along page boundary
            void* fileData = Memory::kPageFrame.AllocateMemory(fileSize, KERNEL_PAGE);
            memcpy(fileData, pData, fileSize);

            // Add to filesystem
            pFiles[FileDescriptors::mouse + count] = File(fileSize, fileData, pHeader->GetName(), count + FileDescriptors::mouse);

            UART::WriteString("[initramfs] Found file ");
            UART::WriteNumber(FileDescriptors::mouse + count);
            UART::WriteString(": ");
            UART::WriteString(pHeader->GetName());
            UART::WriteString("\n");

            // Move to next file
            pHeader = (CPIO::sHeader*)((uint32_t) pData + fileSize);
            pFile = (uint32_t)pHeader;
            count++;
        }
    }

    File* GetFile(const FileDescriptor fd)
    {
        assert(fd < FileDescriptors::MAX_FILES);
        return &pFiles[fd];
    }

    File* GetFile(char const* sName)
    {
        for (unsigned int i = 0; i < FileDescriptors::MAX_FILES; ++i)
        {
            if (strcmp(pFiles[i].m_sName, sName))
                return &pFiles[i];
        }

        assert(false);
        return &pFiles[0];
    }

}
