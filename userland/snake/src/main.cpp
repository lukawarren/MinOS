#include <minlib.h>
#include "events.h"

// Dimensions
constexpr unsigned int nWidth = 300;
constexpr unsigned int nHeight = 300;
constexpr unsigned int nTilesPerDimension = 10;
constexpr unsigned int nTileSize = nWidth / nTilesPerDimension;

// Colours
constexpr unsigned int cEmpty = Graphics::cPanelBackground;
constexpr unsigned int cSnake = 0xffff0000;

// Snake
Vector<Pair<unsigned int, unsigned int>> vSnake;

// Drawing
void DrawFrame();

int main()
{           
    eWindowCreate(nWidth, nHeight, "Snake");
    
    // Init grid
    for (unsigned int x = 0; x < nTilesPerDimension; ++x)
        for (unsigned int y = 0; y < nTilesPerDimension; ++y)
            ePanelCreate(nTileSize, nTileSize, nTileSize*x, nTileSize*y, cEmpty);
    
    // Init snake
    vSnake.Push(new Pair<unsigned int, unsigned int>{0, 0});
    vSnake.Push(new Pair<unsigned int, unsigned int>{1, 0});
    vSnake.Push(new Pair<unsigned int, unsigned int>{2, 0});
    
    DrawFrame();
 
    bool bRunning = true;
    while(bRunning)
    {
        // Get all events
        Pair<bool, Message> message;
        while ((message = Event<>::GetMessage()).m_first)
        {
            auto event = reinterpret_cast<sWindowManagerEvent&>(message.m_second.data);
            const uint32_t pid = message.m_second.sourcePID;
            if (pid != WINDOW_MANAGER_PID) continue;

            switch (event.id)
            {
                case EXIT:
                    bRunning = false;
                break;
                
                case KEY_DOWN:
                {
                    vSnake.Push(new Pair<unsigned int, unsigned int>{3, 0});
                    DrawFrame();
                    break;
                }
                
                case KEY_UP:
                
                break;
                
                default:
                    printf("[Snake] Unrecognised event with id %u\n", event.id);
                    exit(-1);
                break;
            }
        }
        
        if (bRunning) block();
    }
    
    eWindowClose();
    return 0;
}

void DrawFrame()
{
    // Reset all colours
    for (unsigned int x = 0; x < nTilesPerDimension; ++x)
        for (unsigned int y = 0; y < nTilesPerDimension; ++y)
            ePanelColour(y * nTilesPerDimension + x, cEmpty);
    
    // Set snake colors: "x * width + y" because (insert excuse here)
    for (unsigned int i = 0; i < vSnake.Length(); ++i)
        ePanelColour(vSnake[i]->m_first * nTilesPerDimension + vSnake[i]->m_second, cSnake);
}
