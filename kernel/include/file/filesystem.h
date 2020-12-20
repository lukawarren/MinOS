#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>
#include <stddef.h>
#include "file.h"

void BuildVFS(uint32_t vfs);

typedef uint32_t FileHandle;
FileHandle kFileOpen(const char* sName);

uint32_t kGetFileSize(FileHandle file);
char*    kGetFileName(FileHandle file);

void kFileRead(FileHandle file, void* data, uint32_t size = 0);
void kFileClose(FileHandle file);

FileHandle kGetNextFile(FileHandle file);
void PrintFiles();

#endif