#include "graphics.h"
#include "interrupts/syscall.h"
#include "bmp.h"
#include "sseCopy.h"
#include "colours.h"

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

    // Get double buffer
    m_Buffer = malloc(m_Pitch*m_Height);

    // Dirty rects
    memset(m_DirtyRects, 0, sizeof(m_DirtyRects));
    nRects = 0;

    // Draw background
    PushRect(0, 0, m_Width, m_Height, nullptr);
}

void Graphics::DrawRect(uint32_t x, uint32_t y, uint32_t rectWidth, uint32_t rectHeight, uint32_t colour, Window& window)
{
    const uint32_t pitch = window.width * sizeof(uint32_t);

    for (uint32_t row = y; row < y + rectHeight; ++row)
        memset((void*)((uint32_t)window.buffer + row*pitch + x * sizeof(uint32_t)), colour, sizeof(uint32_t)*rectWidth);
}

void Graphics::ConfineWindowToReasonableBounds(Window& window)
{
    // Confine window to reasonable bounds
    if ((int32_t)window.x < 0) window.x = 0;
    if ((int32_t)window.y < 0) window.y = 0;
    if (window.x + window.width >= m_Width) window.x = m_Width-window.width;
    if (window.y + window.height + BAR_HEIGHT >= m_Height) window.y = m_Height-window.height-BAR_HEIGHT;
}

void Graphics::DrawWindow(Window& window)
{
    // Confine window to reasonable bounds
    ConfineWindowToReasonableBounds(window);

    // Draw bar
    DrawRect(0, 0, window.width, BAR_HEIGHT, WINDOW_BAR_COLOUR, window);
    
    // Draw bar text
    uint32_t titleWidth = strlen(window.sName) * CHAR_WIDTH; 
    DrawString(window.sName, window.width / 2 - titleWidth/2, BAR_PADDING, GetColour(255, 255, 255), window);

    PushRect(0, 0, window.width, window.height + BAR_HEIGHT, &window);
}

void Graphics::OnWindowTitleChange(Window& window)
{
    // Draw bar
    DrawRect(0, 0, window.width, BAR_HEIGHT, WINDOW_BAR_COLOUR, window);

    // Draw bar text
    uint32_t titleWidth = strlen(window.sName) * CHAR_WIDTH; 
    DrawString(window.sName, window.width / 2 - titleWidth/2, BAR_PADDING, GetColour(255, 255, 255), window);

    PushRect(0, 0, window.width, BAR_HEIGHT, &window);
}

void Graphics::OnWindowMove(Window& window)
{
    ConfineWindowToReasonableBounds(window);

    // Find directions moved
    const bool bHorizontal = window.x != window.oldX;
    const bool bVertical = window.y != window.oldY;
    const bool bRight = window.x > window.oldX;
    const bool bDown = window.y > window.oldY;

    // Vertical movement
    if (bVertical)
    {
        if (bDown) PushRect(window.x, window.oldY, window.width, window.y - window.oldY, nullptr);
        else PushRect(window.x, window.y + window.height + BAR_HEIGHT, window.width, window.oldY - window.y, nullptr);
    }

    // Horizontal movement
    if (bHorizontal)
    {
        if (bRight) PushRect(window.oldX, window.y, window.x - window.oldX, window.height + BAR_HEIGHT, nullptr);
        else PushRect(window.x + window.width, window.y, window.oldX - window.x, window.height + BAR_HEIGHT, nullptr);
    }

    // Diagonal overlap
    if (bHorizontal && bVertical)
    {
        if (bRight && bDown) PushRect(window.oldX, window.oldY, window.x - window.oldX, window.y - window.oldY, nullptr);
        else if (bRight && !bDown) PushRect(window.oldX, window.y + window.height + BAR_HEIGHT, window.x - window.oldX, window.oldY - window.y, nullptr);
        else if (!bRight && bDown) PushRect(window.x + window.width, window.oldY, window.oldX - window.x, window.y - window.oldY, nullptr);
        else if (!bRight && !bDown) PushRect(window.x + window.width, window.y + window.height + BAR_HEIGHT, window.oldX - window.x, window.oldY - window.y, nullptr);
    }
}

void Graphics::OnWindowDestroy(Window& window)
{
    // First, nullify all existing draw commands refering the window
    for (uint32_t i = 0; i < nRects; ++i)
        if (m_DirtyRects[i].window == &window)
        {
            m_DirtyRects[i].window = nullptr;
            m_DirtyRects[i].width = 0;
            m_DirtyRects[i].height = 0;
        }

    PushRect(window.x, window.y, window.width, window.height + BAR_HEIGHT, nullptr);
}

void Graphics::DrawChar(char c, uint32_t x, uint32_t y, uint32_t colour, Window& window, uint32_t backgroundColour)
{
    const uint8_t* bitmap = GetFontFromChar(c);
    const uint32_t pitch = window.width * sizeof(uint32_t);

    for (int w = 0; w < CHAR_WIDTH; ++w)
    {
        for (int h = 0; h < CHAR_HEIGHT*CHAR_SCALE; ++h)
        {
            uint8_t mask = 1 << (w);

            size_t xPos = x + w;
            size_t yPos = y + h;
            size_t index = xPos*sizeof(uint32_t) + yPos*pitch;
            
            if (bitmap[h/CHAR_SCALE] & mask) *(uint32_t*)((uint32_t)window.buffer + index) = colour; 
            else *(uint32_t*)((uint32_t)window.buffer + index) = backgroundColour;
        }
    }
}

void Graphics::DrawString(char const* string, uint32_t x, uint32_t y, uint32_t colour, Window& window, uint32_t backgroundColour)
{
    for (size_t i = 0; i < strlen(string); ++i) 
    {
        DrawChar(string[i], x + i*CHAR_WIDTH, y, colour, window, backgroundColour);
    }

    PushRect(x, y, strlen(string)*CHAR_WIDTH, CHAR_HEIGHT*CHAR_SCALE, &window);
}

void Graphics::DrawNumber(uint32_t number, uint32_t x, uint32_t y, uint32_t colour, bool hex, Window& window)
{
    // Adjust coordinates
    y += BAR_HEIGHT;

    // Get number of digits
    size_t nDigits = 1;
    size_t i = number;
    while (i/=(hex ? 16 : 10)) nDigits++;

    // Helpers for conversion
    auto digitToASCII = [](const size_t n) { return (char)('0' + n); };
    auto hexToASCII = [](const size_t n) 
    {
        char value = n % 16 + 48;
        if (value > 57) value += 7;
        return value;
    };
    auto getNthDigit = [](const size_t n, const size_t digit, const size_t base) { return int((n / pow(base, digit)) % base); };

    if (hex) DrawString("0x", x, y, colour, window);
    for (size_t d = 0; d < nDigits; ++d)
    { 
        if (hex) DrawChar(hexToASCII(getNthDigit(number, nDigits - d - 1, 16)), x+CHAR_WIDTH*(d+2), y, colour, window);
        else DrawChar(digitToASCII(getNthDigit(number, nDigits - d - 1, 10)), x+CHAR_WIDTH*d, y, colour, window);
    }

    PushRect(x, y, nDigits*CHAR_WIDTH, CHAR_HEIGHT*CHAR_SCALE, &window);
}

void Graphics::DrawFrame(Window* pWindows)
{
    if (nRects == 0) return;

    auto IsRectOccluded = [&](Rect rect)
    {
        // Transform rect coordinates if in window-space
        if (rect.window != nullptr)
        {
            rect.x += rect.window->x;
            rect.y += rect.window->y;
        }
        
        // Iterate through all windows
        Window* window = pWindows;
        while ((uint32_t)window != (uint32_t)nullptr)
        {
            // If window rect is within bounds and differs from rect's window
            if (window != rect.window && !rect.bIgnoreOccluding)
            {
                if (rect.x < window->x + window->width &&                   // Behind X-wise
                    rect.y < window->y + window->height + BAR_HEIGHT  &&    // Behind Y-wise
                    rect.x + rect.width > window->x &&                      // Ahead  X-wise
                    rect.y + rect.height > window->y)                       // Ahead  Y-wise
                {
                    return window; // Return window
                }
            }
            
            window = (Window*) window->pNextWindow;
        }
        
        return (Window*) nullptr;
    };

    auto DrawDirtyRect = [&](Rect& rect)
    {
        if (rect.width == 0 && rect.height == 0) return;
        
        // If rect is occluded, push rect for occluder
        Window* occluder = IsRectOccluded(rect);
        if (occluder != nullptr)
        {
            // Find window with higher z-order
            Window* occlusionWindow = occluder;
            if (rect.window != nullptr && rect.window > occlusionWindow) occlusionWindow = rect.window;
            
            /*
                Behold! The sloppiest, most confusing, and hardest-to-debug
                code in existence! After hours of troubleshooting,
                I realised that "computers are so fast these days", even
                with slow Qemu VRAM, so too bad! I'm redrawing the entire
                window now, and if you don't like it, wait a nanosecond
                or two!
            */
            
            /*
                // Convert rect coords to occluder's window space
                uint32_t rectX = rect.x;
                uint32_t rectY = rect.y;
                if (rect.window != nullptr)
                {
                    rectX += rect.window->x; // Remove previous "window-space"
                    rectY += rect.window->y;
                }
                rectX -= occluder->x;
                rectY -= occluder->y;
                
                // Transform to occlusionWindow's window space
                uint32_t x = rectX + occluder->x - occlusionWindow->x;
                uint32_t y = rectY + occluder->y - occlusionWindow->y;
                
                // Rect collides at least partially with occluder, so that all that need be done is confine this intersection to reasonable bounds.
                uint32_t width = rect.width;
                uint32_t height = rect.height;
                if (x + width > occlusionWindow->width) width = occlusionWindow->width - x;
                if (y + height > occlusionWindow->height + BAR_HEIGHT) height = occlusionWindow->height + BAR_HEIGHT - y;
                
                PushRect(x, y, width, height, occlusionWindow, true);
            */
            
            // Give up and draw the entire occluded window
            PushRect(0, 0, occlusionWindow->width, occlusionWindow->height + BAR_HEIGHT, occlusionWindow, true);
        }

        if (rect.window == nullptr)
        {
            // Draw background pixels instead
            for (uint32_t row = rect.y; row < rect.y + rect.height; ++row)
            {
                uint32_t source = (uint32_t)m_Background + row*m_Pitch + rect.x*sizeof(uint32_t);
                uint32_t destination = (uint32_t)m_Buffer + row*m_Pitch + rect.x*sizeof(uint32_t);

                memcpy((void*)destination, (void*)source, sizeof(uint32_t)*rect.width);
            }
            
            return;
        }

        for (uint32_t row = rect.y; row < rect.y + rect.height; ++row)
        {
            uint32_t screenX = rect.x + rect.window->x;
            uint32_t screenY = row + rect.window->y;
            uint32_t windowPitch = rect.window->width * sizeof(uint32_t);

            uint32_t source = (uint32_t)rect.window->buffer + row*windowPitch + rect.x*sizeof(uint32_t);
            uint32_t destination = (uint32_t)m_Buffer + screenY*m_Pitch + screenX*sizeof(uint32_t);

            for (uint32_t i = 0; i < sizeof(uint32_t)*rect.width; i += sizeof(uint32_t))
                *((uint32_t*)(destination+i)) = *((uint32_t*)(source+i));
        }
        
    };

    // For double buffering
    auto BlitDirtyRect = [&](Rect& rect)
    {
        if (rect.width == 0 && rect.height == 0) return;

        uint32_t offsetX = 0;
        uint32_t offsetY = 0;

        if (rect.window != nullptr) { offsetX = rect.window->x; offsetY = rect.window->y; }

        // Draw background pixels instead
        for (uint32_t row = rect.y; row < rect.y + rect.height; ++row)
        {
            const uint32_t x = rect.x + offsetX;
            const uint32_t y = row + offsetY;
            
            const uint32_t source = (uint32_t)m_Buffer + y*m_Pitch + x*sizeof(uint32_t);
            const uint32_t destination = m_Address + y*m_Pitch + x*sizeof(uint32_t);
            
            for (uint32_t i = 0; i < sizeof(uint32_t)*rect.width; i += sizeof(uint32_t))
                *((uint32_t*)(destination+i)) = *((uint32_t*)(source+i));
        }
    };

    // Draw dirty rects
    for (uint32_t i = 0; i < nRects; ++i) DrawDirtyRect(m_DirtyRects[i]);
    for (uint32_t i = 0; i < nRects; ++i) BlitDirtyRect(m_DirtyRects[i]);

    // Clear dirty rects
    nRects = 0;
}

Graphics::~Graphics() {}
