#pragma once
#ifndef FILE_H
#define FILE_H

struct File
{
    char sName[256];
    uint32_t size;
    File* pNext = nullptr;
};

struct Filesystem
{
    File* pFile;
};

#endif