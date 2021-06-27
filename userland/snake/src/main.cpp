#include <minlib.h>
#include "events.h"
#include "keyboard.h"

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
enum Direction { UP, DOWN, LEFT, RIGHT } eDirection;
unsigned int nSegments = 0;

// Routines
void HandleInput(uint8_t scancode);
bool GameOver();
void AdvanceSnake();
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
    eDirection = RIGHT;
    nSegments = 3;
 
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
                    eKeyDown* keyEvent = (eKeyDown*) event.data;
                    HandleInput(keyEvent->scancode);
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
        
        if (!GameOver())
        {   
            AdvanceSnake();
            DrawFrame();
        } else block();
    }
    
    eWindowClose();
    return 0;
}

void HandleInput(uint8_t scancode)
{
    if (scancode == Input::Keyboard::Code::W)
        eDirection = Direction::UP;
    
    if (scancode == Input::Keyboard::Code::A)
        eDirection = Direction::LEFT;
        
    if (scancode == Input::Keyboard::Code::S)
        eDirection = Direction::DOWN;
        
    if (scancode == Input::Keyboard::Code::D)
        eDirection = Direction::RIGHT;
}

bool GameOver()
{
    auto headX = vSnake[nSegments-1]->m_first;
    auto headY = vSnake[nSegments-1]->m_second;
    
    switch (eDirection)
    {
        case Direction::LEFT:
            return headX == 0;
        
        case Direction::RIGHT:
            return headX >= nTilesPerDimension-1;
        
        case Direction::UP:
            return headY == 0;
        
        default: // Down
            return headY >= nTilesPerDimension-1;
    }
}

void AdvanceSnake()
{
    auto headX = vSnake[nSegments-1]->m_first;
    auto headY = vSnake[nSegments-1]->m_second;
    
    switch (eDirection)
    {
        case Direction::LEFT:
            vSnake.Push(new Pair<unsigned int, unsigned int>{headX-1, headY});
        break;
        
        case Direction::RIGHT:
            vSnake.Push(new Pair<unsigned int, unsigned int>{headX+1, headY});
        break;
        
        case Direction::UP:
            vSnake.Push(new Pair<unsigned int, unsigned int>{headX, headY-1});
        break;
        
        default: // Down
            vSnake.Push(new Pair<unsigned int, unsigned int>{headX, headY+1});
        break;
    }
    
    vSnake.Pop(vSnake[0]);
}

void DrawFrame()
{    
    const auto IsSnakeTile = [&](const unsigned int x, const unsigned int y)
    {
        for (unsigned int i = 0; i < vSnake.Length(); ++i)
            if (vSnake[i]->m_first == x && vSnake[i]->m_second == y)
                return true;
        
        return false;
    };
    
    // Reset non-snake colours
    for (unsigned int x = 0; x < nTilesPerDimension; ++x)
        for (unsigned int y = 0; y < nTilesPerDimension; ++y)
            if (!IsSnakeTile(x, y))
                ePanelColour(y * nTilesPerDimension + x, cEmpty);
    
    // Set snake colors: "x * width + y" because (insert excuse here)
    for (unsigned int i = 0; i < vSnake.Length(); ++i)
        ePanelColour(vSnake[i]->m_first * nTilesPerDimension + vSnake[i]->m_second, cSnake);
}
