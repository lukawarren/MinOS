#include <minlib.h>
#include "compositor.h"
#include "mouse.h"
#include "events.h"

int main()
{
    Graphics::Compositor compositor;
    Input::Mouse mouse = Input::Mouse(compositor.m_screenWidth / 2, compositor.m_screenHeight / 2);
    
    while (1)
    {
        // Process all events
        Pair<bool, Message> message;
        while ((message = Event<>::GetMessage()).m_first)
        {
            auto event = reinterpret_cast<sWindowManagerEvent&>(message.m_second.data);
            
            switch (event.id)
            {
                // Window creation
                case WINDOW_CREATE:
                {
                    eWindowCreate* createWindowEvent = (eWindowCreate*) event.data;
                    
                    auto window = new Graphics::Window(createWindowEvent->width, createWindowEvent->height, 0, 0, createWindowEvent->sName);
                    compositor.m_vWindows.Push(window);
                    compositor.DrawRegion(window->m_X, window->m_Y, window->m_Width, window->m_Height);
                    
                    break;
                }
                
                default:
                    printf("Unrecognised event with id %u\n", event.id);
                break;
            }
        }
        
        compositor.DrawMouse(mouse);
    }
    
    return 0;
}
