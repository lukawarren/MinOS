#pragma once
#ifndef FILE_H
#define FILE_H

#define MAX_FILE_NAME_LENGTH 32

struct File
{
    char sName[MAX_FILE_NAME_LENGTH];
    uint32_t size;
    uint32_t pNext = 0;
}  __attribute__((packed));

struct Filesystem
{
    File* pFile;
};

#endif