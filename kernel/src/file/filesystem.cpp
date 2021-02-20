#include "filesystem.h"
#include "../gfx/vga.h"
#include "../memory/paging.h"

static uint32_t sFilesystem;

void BuildVFS(uint32_t vfs)
{
    // Correct linked list within filesystem
    // to account for memory offset
    File* file = (File*)vfs;

    while (file != nullptr)
    {
        if ((File*)file->pNext == nullptr) file = nullptr;
        else
        {
            file->pNext = vfs + file->pNext;
            file = (File*)file->pNext;
        }
    }

    sFilesystem = vfs;
}

FileHandle kFileOpen(const char* sName)
{
    // Find file and return relative offset as handle
    File* file = (File*)sFilesystem;

    while (file != nullptr)
    {
        if (strcmp(sName, file->sName)) return (uint32_t)file - sFilesystem;
        file = (File*)file->pNext;
    }    

    return -1;
}

uint32_t kGetFileSize(FileHandle file)
{
    return ((File*)(sFilesystem + file))->size;
}

char* kGetFileName(FileHandle file)
{
    return ((File*)(sFilesystem + file))->sName;
}

void kFileRead(FileHandle file, void* data, uint32_t size)
{
    if (size == 0) size = kGetFileSize(file);

    void* fileData = (void*)(sFilesystem + file + sizeof(File));
    memcpy(data, fileData, size);
}

void kFileClose(FileHandle file __attribute__((unused))) {}

FileHandle kGetFirstFile()
{
    return 0;
}

FileHandle kGetNextFile(FileHandle file)
{
    File* nextFile = (File*)((File*)(sFilesystem + file))->pNext;
    if (nextFile == nullptr) return -1;
    return (uint32_t)nextFile - sFilesystem;
}

void PrintFiles()
{
    FileHandle file = 0;
    while (file != (FileHandle)-1)
    {
        VGA_printf("-- ", false);
        VGA_printf(kGetFileName(file));
        file = kGetNextFile(file);
    }
}