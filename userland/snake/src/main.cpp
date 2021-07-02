#include <minlib.h>
#include "events.h"
#include "keyboard.h"

// Dimensions
constexpr unsigned int nWidth = 600;
constexpr unsigned int nHeight = 600;
constexpr unsigned int nTilesPerDimension = 20;
constexpr unsigned int nTileSize = nWidth / nTilesPerDimension;

// Colours
constexpr unsigned int cEmpty = Graphics::cPanelBackground;
constexpr unsigned int cSnake = 0xffff0000;
constexpr unsigned int cHead =  0xff00ff00;
constexpr unsigned int cFood =  0xffffff00;

// Snake
Vector<Pair<unsigned int, unsigned int>> vSnake;
enum Direction { UP, DOWN, LEFT, RIGHT } eDirection;
unsigned int nSegments = 0;

// Drawing
Pair<unsigned int, unsigned int> lastTile;
bool bDrawLastTile;
bool bDrewInitialSnake;

// Food
Vector<Pair<unsigned int, unsigned int>> vFood;

// Routines
void HandleInput(uint8_t scancode);
bool GameOver();
void AdvanceSnake();
void DrawFrame();
void SpawnFood();

int main()
{           
    eWindowCreate(nWidth, nHeight, "Snake");
    
    // Init grid
    for (unsigned int y = 0; y < nTilesPerDimension; ++y)
        for (unsigned int x = 0; x < nTilesPerDimension; ++x)
            ePanelCreate(nTileSize, nTileSize, nTileSize*x, nTileSize*y, cEmpty);
    
    // Init snake
    vSnake.Push(new Pair<unsigned int, unsigned int>{0, 0});
    vSnake.Push(new Pair<unsigned int, unsigned int>{1, 0});
    vSnake.Push(new Pair<unsigned int, unsigned int>{2, 0});
    bDrewInitialSnake = false;
    eDirection = RIGHT;
    nSegments = 3;
    
    SpawnFood();
    DrawFrame();
 
    EventLoop<sWindowManagerEvent>([&](const sWindowManagerEvent event, const bool bEvent)
    {
        // Deal with events
        if (bEvent)
        {
            switch (event.id)
            {
                case EXIT:
                    return false;
                
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
        
        // Progress game
        static int nFrame = 0;
        if (!GameOver())
        {   
            if (++nFrame % 50 == 0)
            {
                AdvanceSnake();
                DrawFrame();
            }
        }
        
        usleep(1000);
        return true;
    }, false);
    
    eWindowClose();
    return 0;
}

void HandleInput(uint8_t scancode)
{
    if (scancode == Input::Keyboard::Code::W && eDirection != Direction::DOWN)
        eDirection = Direction::UP;
    
    else if (scancode == Input::Keyboard::Code::A && eDirection != Direction::RIGHT)
        eDirection = Direction::LEFT;
        
    else if (scancode == Input::Keyboard::Code::S && eDirection != Direction::UP)
        eDirection = Direction::DOWN;
        
    else if (scancode == Input::Keyboard::Code::D && eDirection != Direction::LEFT)
        eDirection = Direction::RIGHT;
}

bool GameOver()
{
    auto headX = vSnake[nSegments-1]->m_first;
    auto headY = vSnake[nSegments-1]->m_second;
    
    // Check for self-collision
    for (size_t i = 0; i < vSnake.Length()-1; ++i)
        if (vSnake[i]->m_first == headX && vSnake[i]->m_second == headY)
            return true;
    
    // Check for going outside of bounds
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
    
    // Advance snake
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
    
    // If our head's colliding with food, don't shrink
    for (unsigned int i = 0; i < vFood.Length(); ++i)
    {
        if (vFood[i]->m_first == headX && vFood[i]->m_second == headY)
        {
            // Destroy old food, make new one
            vFood.Pop(vFood[i]);
            SpawnFood();
            
            // Increase segments and avoid dirtying tile
            bDrawLastTile = false;
            nSegments++;
            return;
        }
    }
    
    // Pop last segment off snake
    lastTile = *vSnake[0];
    vSnake.Pop(vSnake[0]);
    bDrawLastTile = true;
}

void DrawFrame()
{    
    // Reset non-snake colour for dirty tile
    if (bDrawLastTile)
        ePanelColour(lastTile.m_second * nTilesPerDimension + lastTile.m_first, cEmpty);

    if (!bDrewInitialSnake)
    {
        // Draw all of snake...
        for (unsigned int i = 0; i < vSnake.Length(); ++i)
            ePanelColour(vSnake[i]->m_second * nTilesPerDimension + vSnake[i]->m_first, i == vSnake.Length() -1 ? cHead : cSnake);
            
        bDrewInitialSnake = true;
    }
    else
    {
        // ...or just the head and the old one
        auto headX = vSnake[nSegments-1]->m_first;
        auto headY = vSnake[nSegments-1]->m_second;
        ePanelColour(headY * nTilesPerDimension + headX, cHead);
        
        auto oldHeadX = vSnake[nSegments-2]->m_first;
        auto oldHeadY = vSnake[nSegments-2]->m_second;
        ePanelColour(oldHeadY * nTilesPerDimension + oldHeadX, cSnake);
        
    }
    
    // Set food colors
    for (unsigned int i = 0; i < vFood.Length(); ++i)
        ePanelColour(vFood[i]->m_second * nTilesPerDimension + vFood[i]->m_first, cFood);
}

void SpawnFood()
{
    const auto IsSnakeTile = [&](const Pair<unsigned int, unsigned int>* tile)
    {
        for (unsigned int i = 0; i < vSnake.Length(); ++i)
            if (vSnake[i]->m_first == tile->m_first && vSnake[i]->m_second == tile->m_second)
                return true;
            
        return false;  
    };
    
    auto coord = new Pair<unsigned int, unsigned int> { rand() % nTilesPerDimension, rand() % nTilesPerDimension };
    while (IsSnakeTile(coord)) *coord = { rand() % nTilesPerDimension, rand() % nTilesPerDimension };
    vFood.Push(coord);
}
