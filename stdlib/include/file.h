#pragma once
#ifndef FILE_H
#define FILE_H

struct File
{
    char sName[32];
    uint32_t size;
    File* pNext = nullptr;
}  __attribute__((packed));

struct Filesystem
{
    File* pFile;
};

#endif