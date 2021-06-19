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
        
        m_sState.x = x;
        m_sState.y = y;
        m_sState.bLeftButton = false;
        m_sState.bRightButton = false;
    }

    Mouse::MouseState Mouse::UpdateState(int maxWidth, int maxHeight)
    {
        // Read mouse coords and buttons, then set coords to 0
        m_sState.x += m_pMouse[0];
        m_sState.y -= m_pMouse[1];
        m_sState.bLeftButton = m_pMouse[2];
        m_sState.bRightButton = m_pMouse[3];
        m_pMouse[0] = 0;
        m_pMouse[1] = 0;
        
        // Restrain to reasonable bounds
        m_sState.x = MAX(MIN(m_sState.x, maxWidth),  0);
        m_sState.y = MAX(MIN(m_sState.y, maxHeight), 0);
        
        return m_sState;
    }

    Mouse::~Mouse()
    {
        munmap(m_pMouse, m_fileSize);
        fclose(m_pFile);
    }
}
