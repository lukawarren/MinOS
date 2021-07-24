#pragma once
#ifndef BITMAP_H
#define BITMAP_H

#include <minlib.h>

namespace Graphics
{
    class Bitmap
    {
    public:
        Bitmap(char const* sPath);
        ~Bitmap();
        
        uint32_t m_width;
        uint32_t m_height;
        uint32_t* m_pImage;
        
    private:
        FILE* m_pFile;
        uint32_t m_fileSize;
        void* m_pFileData;
        
        struct sHeader
        {
            char magicBytes[2]; // BM for Widows 3.1x, 95, NT, ... etc
            uint32_t fileSize;
            uint32_t reserved;
            uint32_t imageDataOffset;
        } __attribute__((packed));
        
        struct sInfoHeader
        {
            uint32_t headerSize; // Always equals 40
            int32_t width;
            int32_t height;
            uint16_t colourPlanes; // Must be 1
            uint16_t colourDepth;
            uint32_t compression; // We want value 0 - BI_RGB
        } __attribute__((packed));
    };
}

#endif
