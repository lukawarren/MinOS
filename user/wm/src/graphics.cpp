#include "graphics.h"
#include "interrupts/syscall.h"
#include "bmp.h"
#include "sseCopy.h"

Graphics::Graphics() {}

void Graphics::Init(uint32_t width, uint32_t height, uint32_t address, uint32_t pitch)
{
    m_Width = width; m_Height = height;
    m_Address = address; m_Pitch = pitch;

    // Create background buffer
    m_Background = malloc(m_Pitch*m_Height);

    // Load image
    FileHandle file = fileOpen("background.bmp");
    void* data = malloc(getFileSize(file));
    fileRead(file, data, 0);

    // Parse bitmap
    auto bitmap = ParseBitmap((uint32_t)data);
    if (!bitmap.error) memcpy(m_Background, (void*)bitmap.address, m_Pitch*m_Height);
    else memset(m_Background, 0, m_Pitch*height);

    // Clean up
    free((void*)bitmap.address, bitmap.size);
    free(data, getFileSize(file));
    fileClose(file);

    // Create regular buffer
    m_Buffer = malloc(m_Pitch*m_Height);

    // Dirty rows
    m_DirtyRows = (bool*) malloc(sizeof(bool)*m_Height);
}

void Graphics::DrawRect(uint32_t x, uint32_t y, uint32_t rectWidth, uint32_t rectHeight, uint32_t colour)
{
    /*
        I tried doing a bunch of fancy optimising
        but the compiler beat me to it!
        In other words, there's no use! 
    */

    for (uint32_t i = x; i < x + rectWidth; ++i)
        for (uint32_t j = y; j < y + rectHeight; ++j)
            PutPixel(i, j, colour);
}

void Graphics::DrawBackground()
{
    Blit(m_Background);
    memset(m_DirtyRows, 1, sizeof(bool)*m_Height);
}

void Graphics::DrawWindow(const char* sTitle, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    const uint32_t barHeight = 20;
    const uint32_t barPadding = 3;

    // Draw outlilne and bar
    DrawRect(x, y - barHeight, width, barHeight, GetColour(68, 68, 68));

    // Draw window
    DrawRect(x, y, width, height, GetColour(51, 51, 51));

    // Draw bar text
    DrawString(sTitle, x + barPadding, y - barHeight + barPadding, GetColour(255, 255, 255));

    for (uint32_t row = y - barHeight; row < y+height+barHeight; ++row)
        m_DirtyRows[row] = 1;
}

void Graphics::DrawChar(char c, uint32_t x, uint32_t y, uint32_t colour)
{
    const uint8_t* bitmap = GetFontFromChar(c);

    for (int w = 0; w < CHAR_WIDTH; ++w)
    {
        for (int h = 0; h < CHAR_HEIGHT*CHAR_SCALE; ++h)
        {
            uint8_t mask = 1 << (w);

            size_t xPos = x + w;
            size_t yPos = y + h;
            size_t index = xPos*4 + yPos*m_Pitch;
            
            if (bitmap[h/CHAR_SCALE] & mask) *(uint32_t*)((uint32_t)m_Buffer + index) = colour; 
        }
    }
}

void Graphics::DrawString(char const* string, uint32_t x, uint32_t y, uint32_t colour)
{
    for (size_t i = 0; i < strlen(string); ++i) 
    {
        DrawChar(string[i], x + i*CHAR_WIDTH, y, colour);
    }
}

void Graphics::Blit(void* data)
{   
    MemcpySSE((void*)m_Buffer, data, m_Pitch*m_Height);
}

void Graphics::SwapBuffers()
{
    for (uint32_t row = 0; row < m_Height; ++row)
        if (m_DirtyRows[row])
            MemcpySSE((void*)(m_Address + row*m_Pitch), (void*)((uint32_t)m_Buffer + row*m_Pitch), m_Pitch);

    memset(m_DirtyRows, 0, sizeof(bool)*m_Height);
}

Graphics::~Graphics() {}