#pragma once
#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stdint.h>
#include <stddef.h>

struct File
{
    char sName[256];
    File* pNext = nullptr;
    uint32_t size;
};

struct Folder
{
    char sName[256];
    File* pFile;
};



#endif