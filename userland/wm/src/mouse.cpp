#include "mouse.h"
#include <minlib.h>

namespace Input
{
    Mouse::Mouse(const int x, const int y)
    {
        // Get mouse file
        m_pFile = fopen("/dev/mouse", "w+");

        // Get mouse size
        struct stat mouseStat;
        fstat(m_pFile->_file, &mouseStat);
        m_fileSize = mouseStat.st_size;

        // Map file
        m_pMouse = (int*) mmap(NULL, m_fileSize, PROT_WRITE | PROT_READ, MAP_SHARED, m_pFile->_file, 0);
        
        m_sPosition.x = x;
        m_sPosition.y = y;
    }

    Mouse::MousePosition Mouse::UpdatePosition(int maxWidth, int maxHeight)
    {
        // Read mouse coords, then set to 0
        m_sPosition.x += m_pMouse[0];
        m_sPosition.y -= m_pMouse[1];
        m_pMouse[0] = 0;
        m_pMouse[1] = 0;
        
        // Restrain to reasonable bounds
        m_sPosition.x = MAX(MIN(m_sPosition.x, maxWidth),  0);
        m_sPosition.y = MAX(MIN(m_sPosition.y, maxHeight), 0);
        
        return m_sPosition;
    }

    Mouse::~Mouse()
    {
        munmap(m_pMouse, m_fileSize);
        fclose(m_pFile);
    }
}
