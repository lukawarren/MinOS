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

        int count = 0;
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

            // Add to filesystem
            pFiles[FileDescriptors::wm + count] = File(fileSize, pData, pHeader->GetName(), count + FileDescriptors::wm);

            UART::WriteString("[initramfs] Found program ");
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
        assert(fd < FileDescriptors::N_FILES);
        return &pFiles[fd];
    }

    File* GetFile(char const* sName)
    {
        for (unsigned int i = 0; i < FileDescriptors::N_FILES; ++i)
        {
            if (strcmp(pFiles[i].m_sName, sName))
                return &pFiles[i];
        }

        assert(false);
        return &pFiles[0];
    }

}
