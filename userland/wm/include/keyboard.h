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
            char ScancodeToCharacter(const uint8_t scancode) const;
            
            uint8_t m_buffer[128];
            uint8_t m_oldBuffer[128];
            bool m_bShift;
            
            enum Code
            {
                W = 17,
                S = 31,
                A = 30,
                D = 32,
                Shift = 42,
                Comma = 51,
                FullStop = 52,
                Space = 57
            };
            
        private:
            FILE* m_pFile;
            uint32_t m_fileSize;
            int* m_pKeyboard;
    };
}

#endif
