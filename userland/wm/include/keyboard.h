#pragma once
#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <minlib.h>

namespace Input
{
    class Keyboard
    {
        public:
            Keyboard();
            ~Keyboard();
        
            void Poll();
            
            uint8_t m_buffer[128];
            uint8_t m_oldBuffer[128];
            
            enum Code
            {
                W = 17,
                S = 31,
                A = 30,
                D = 32,
                SPACE = 57
            };
            
        private:
            FILE* m_pFile;
            uint32_t m_fileSize;
            int* m_pKeyboard;
    };
}

#endif
