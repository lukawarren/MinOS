#pragma once
#ifndef CPIO_H
#define CPIO_H

#include <stdint.h>
#include <stddef.h>
#include "kstdlib.h"

#define CPIO_MODE_FILE 100000

namespace CPIO
{
    struct sHeader
    {
        char magic[6];      // Should be 070707
        char device[6];     // Device
        char ino[6];        // Inode
        char mode[6];       // Mode - permissions and file type
        char uid[6];        // UID
        char gid[6];        // GID
        char nLinks[6];     // Number of links to file
        char rdev[6];       // For block devices and what-not, contains the device number
        char mtime[11];     // Last modified time
        char nameSize[6];   // Size of name string to come
        char fileSize[11];  // File size

        uint32_t OctalToDecimal(uint32_t number) const
        {
            unsigned int nDecimal = 0, i = 0;

            while (number != 0)
            {
                nDecimal += (number % 10) * pow(8, i);
                number /= 10;
                ++i;
            }

            return nDecimal;
        }

        uint32_t StringToDecimal(const char* string, const uint32_t length) const
        {
            uint32_t value = 0;
            uint32_t count = 0;
            for(const char* c = string; *c != '\0' && *c >= '0' && *c <= '9' && count < length; ++c)
            {
                value = (*c - '0') + value*10;
                ++count;
            }
            return OctalToDecimal(value);
        }

        bool ConformsToMagic() const
        {
            return
                magic[0] == '0' &&
                magic[1] == '7' &&
                magic[2] == '0' &&
                magic[3] == '7' &&
                magic[4] == '0' &&
                magic[5] == '7';
        }

        bool IsFile() const
        {
            return StringToDecimal(mode, 6) & CPIO_MODE_FILE;
        }

        char const* GetName() const
        {
            return (char const*)(this+1);
        }

        bool IsTerminator() const
        {
            return strcmp(GetName(), "TRAILER!!!");
        }
    };
}

#endif