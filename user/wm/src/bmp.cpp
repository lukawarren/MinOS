#include "bmp.h"
#include "interrupts/syscall.h"

struct BMPHeader
{
    char magicBytes[2]; // BM for Widows 3.1x, 95, NT, ... etc
    uint32_t fileSize;
    uint32_t reserved;
    uint32_t imageDataOffset;
} __attribute__((packed));

struct BMPInfoHeader
{
    uint32_t headerSize; // Always equals 40
    int32_t width;
    int32_t height;
    uint16_t colourPlanes; // Must be 1
    uint16_t colourDepth;
    uint32_t compression; // We want value 0 - BI_RGB
} __attribute__((packed));

Bitmap ParseBitmap(uint32_t address)
{
    // Find bmp header
    BMPHeader* header = (BMPHeader*)address;
    if (header->magicBytes[0] != 'B' || header->magicBytes[1] != 'M') return {0, 0, 1};

    // Get dimensions and other info
    BMPInfoHeader* info = (BMPInfoHeader*)(address + sizeof(BMPHeader));
    uint32_t width = (uint32_t) info->width;
    uint32_t height = (uint32_t) info->height;

    if (info->colourPlanes != 1 || info->colourDepth != 24 || info->compression != 0) return {0, 0, 1};

    // No padding support
    if ((info->width * 3) % 4 != 0 || (info->height * 3) % 4 != 0) return {0, 0, 1};

    // Allocate memory
    void* output = malloc(width*height*sizeof(uint32_t));

    // Get pixels
    uint8_t* pixels = (uint8_t*)(address + header->imageDataOffset);

    auto PutPixel = [&](size_t x, size_t y, uint32_t colour)
    {
        size_t index = x*4 + y*width*4;
        *(uint32_t*)((uint32_t)output + index) = colour;
    };
    
    auto GetColour = [](uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xff)
    {
        return (uint32_t)(a << 24 | r << 16 | g << 8 | b);
    };

    for (uint32_t x = 0; x < width; ++x)
    {
        for (uint32_t y = 0; y < height; ++y)
        {
            uint8_t red     = pixels[y*width*3+x*3+2];
            uint8_t green   = pixels[y*width*3+x*3+1];
            uint8_t blue    = pixels[y*width*3+x*3+0];
            PutPixel(width - x - 1, height - y - 1, GetColour(red, green, blue));
        }
    }

    return Bitmap {(uint32_t) output, width*height*4, 0};
}