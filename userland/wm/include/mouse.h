#pragma once
#ifndef MOUSE_H
#define MOUSE_H

#include <minlib.h>

#define MOUSE_BITMAP_WIDTH 8
#define MOUSE_BITMAP_HEIGHT 8
#define MOUSE_BITMAP \
"\
00011000\
00011000\
00111100\
01111110\
01111110\
01111110\
00011000\
00011000\
"

namespace Input
{
    class Mouse
    {
        public:
            Mouse(const int x, const int y);
            ~Mouse();
        
            struct MouseState
            {
                int x;
                int y;
                bool bLeftButton;
                bool bRightButton;
            };
        
            MouseState UpdateState(int maxWidth, int maxHeight);
            MouseState m_sState;
            
        private:
            FILE* m_pFile;
            uint32_t m_fileSize;
            int* m_pMouse;
    };
}

#endif
