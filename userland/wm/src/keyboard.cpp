#include "keyboard.h"
#include <minlib.h>

namespace Input
{
    Keyboard::Keyboard()
    {
        // Get keyboard file
        m_pFile = fopen("/dev/keyboard", "w+");

        // Get keyboard size
        struct stat keyboardStat;
        fstat(m_pFile->_file, &keyboardStat);
        m_fileSize = keyboardStat.st_size;

        // Map file
        m_pKeyboard = (int*) mmap(NULL, m_fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, m_pFile->_file, 0);
        
        m_bShift = false;
    }

    void Keyboard::Poll()
    {
        // Save old state
        memcpy(m_oldBuffer, m_buffer, sizeof(m_buffer));
        
        // Retrieve new state
        memcpy(m_buffer, m_pKeyboard, sizeof(m_buffer));
        
        // Deal with modifier keys
        m_bShift = m_buffer[Code::Shift];
    }

    char Keyboard::ScancodeToCharacter(const uint8_t scancode) const
    {
        static char const* topNumbers =   "!\"#$%^&*(";
        static char const* numbers =      "123456789";
        static char const* qwertzuiop =   "qwertyuiopQWERTYUIOP";
        static char const* asdfghjkl =    "asdfghjklASDFGHJKL";
        static char const* yxcvbnm =      "zxcvbnmZXCVBNM";
        
        // Standard characters
        if (scancode >= 0x2 && scancode <= 0xa)
        {
            if (m_bShift) return topNumbers[scancode - 0x2];
            else return numbers[scancode - 0x2];
        }
        else if(scancode >= 0x10 && scancode <= 0x1C) return qwertzuiop[scancode - 0x10 + m_bShift*10];
        else if(scancode >= 0x1E && scancode <= 0x26) return asdfghjkl[scancode - 0x1E + m_bShift*9];
        else if(scancode >= 0x2C && scancode <= 0x32) return yxcvbnm[scancode - 0x2C + m_bShift*7];

        // Special characters
        else if (scancode == Code::Space) return ' ';
        else if (scancode == Code::Comma) return m_bShift ? '<' : ',';
        else if (scancode == Code::FullStop) return m_bShift ? '>' : '.';

        return '\0';
    }

    Keyboard::~Keyboard()
    {
        munmap(m_pKeyboard, m_fileSize);
        fclose(m_pFile);
    }
}
