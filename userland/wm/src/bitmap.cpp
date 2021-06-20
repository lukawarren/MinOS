#include "bitmap.h"

Graphics::Bitmap::Bitmap(char const* sPath)
{
    // Get image
    m_pFile = fopen(sPath, "w+");

    // Get image size
    struct stat imageStat;
    fstat(m_pFile->_file, &imageStat);
    m_fileSize = imageStat.st_size;

    // Map image as a BMP header and verify format
    m_pFileData = mmap(NULL, m_fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, m_pFile->_file, 0);
    
    // Find BMP header and verify format
    sHeader* pHeader = (sHeader*)m_pFileData;
    assert(pHeader->magicBytes[0] == 'B' && pHeader->magicBytes[1] == 'M'); // Windows BMP format
    
    // Get dimensions and check details
    sInfoHeader* pInfoHeader = (sInfoHeader*)(pHeader+1);
    m_width = (uint32_t) pInfoHeader->width;
    m_height = (uint32_t) pInfoHeader->height;
    assert(pInfoHeader->colourPlanes == 1 && pInfoHeader->colourDepth == 24 && pInfoHeader->compression == 0);
    
    // Padding not currently supported
    if ((m_width * 3) % 4 != 0 || ((m_height * 3) % 4) != 0) assert(false);
    
    // Alloc memory
    m_pImage = (uint32_t*) malloc(sizeof(uint32_t) * m_width * m_height);
    
    // Find where pixels begin and parse
    uint8_t* pPixels = (uint8_t*)((uint32_t)pHeader + pHeader->imageDataOffset);
    
    for (uint32_t x = 0; x < m_width; ++x)
    {
        for (uint32_t y = 0; y < m_height; ++y)
        {
            uint8_t red     = pPixels[y*m_width*3+x*3+2];
            uint8_t green   = pPixels[y*m_width*3+x*3+1];
            uint8_t blue    = pPixels[y*m_width*3+x*3+0];
            
            // Correcting for y being flipped
            m_pImage[(m_height-y-1) * m_width + x] = 0xff000000 | (red << 16) | (green << 8) | blue;
        }
    }
    
}

Graphics::Bitmap::~Bitmap()
{
    munmap(m_pFileData, m_fileSize);
    fclose(m_pFile);
    free(m_pImage);
}
