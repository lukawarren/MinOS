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
    }

    void Keyboard::Poll()
    {
        // Save old state
        memcpy(m_oldBuffer, m_buffer, sizeof(m_buffer));
        
        // Retrieve new state
        memcpy(m_buffer, m_pKeyboard, sizeof(m_buffer));
    }

    Keyboard::~Keyboard()
    {
        munmap(m_pKeyboard, m_fileSize);
        fclose(m_pFile);
    }
}
