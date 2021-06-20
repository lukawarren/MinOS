#include <minlib.h>
#include "compositor.h"
#include "mouse.h"
#include "events.h"
#include "panel.h"
#include "text.h"

int main()
{
    Graphics::Compositor compositor;
    Input::Mouse mouse = Input::Mouse(compositor.m_screenWidth / 2, compositor.m_screenHeight / 2);
    
    loadprogram("notepad/notepad.bin");
    
    while (1)
    {
        // Process all events
        Pair<bool, Message> message;
        while ((message = Event<>::GetMessage()).m_first)
        {
            auto event = reinterpret_cast<sWindowManagerEvent&>(message.m_second.data);
            const uint32_t pid = message.m_second.sourcePID;

            switch (event.id)
            {
                // Window creation
                case WINDOW_CREATE:
                {
                    // If a window for this PID already exists, abandon ship!
                    if (compositor.GetWindowForPID(pid) != nullptr) break;

                    eWindowCreate* createWindowEvent = (eWindowCreate*) event.data;
                    
                    auto window = new Graphics::Window
                    (
                        createWindowEvent->width, createWindowEvent->height,
                        compositor.m_screenWidth / 2 - createWindowEvent->width / 2,
                        compositor.m_screenHeight / 2 - createWindowEvent->height / 2,
                        createWindowEvent->sName,
                        pid
                    );
                    
                    compositor.m_vWindows.Push(window);
                    compositor.DrawRegion(window->m_X, window->m_Y, window->m_Width, window->m_Height);
                    break;
                }
                
                case WINDOW_CLOSE:
                {
                    auto window = compositor.GetWindowForPID(pid);
                    if (window ==  nullptr) break;
                    
                    // Save dimensions
                    auto x = window->m_X;
                    auto y = window->m_Y;
                    auto width = window->m_Width;
                    auto height = window->m_Height;
                    
                    // Delete window
                    compositor.m_vWindows.Pop(window);
                    
                    // Draw over newly empty space
                    compositor.DrawRegion(x, y, width, height);
                    
                    break;
                }

                case PANEL_CREATE:
                {
                    auto window = compositor.GetWindowForPID(pid);
                    if (window == nullptr) break;
                    
                    ePanelCreate* createPanelEvent = (ePanelCreate*) event.data;

                    auto panel = new Graphics::Panel
                    (
                        createPanelEvent->width,
                        createPanelEvent->height,
                        createPanelEvent->x,
                        createPanelEvent->y,
                        createPanelEvent->colour
                    );
                    
                    window->AddWidget(panel);
                    compositor.DrawRegion(window->m_X + panel->m_X, window->m_Y + panel->m_Y, panel->m_Width, panel->m_Height);
                    break;
                }
                
                case TEXT_CREATE:
                {
                    auto window = compositor.GetWindowForPID(pid);
                    if (window == nullptr) break;
                    
                    eTextCreate* createTextEvent = (eTextCreate*) event.data;

                    auto text = new Graphics::Text
                    (
                        createTextEvent->text,
                        createTextEvent->x,
                        createTextEvent->y,
                        createTextEvent->colour
                    );
                    
                    window->AddWidget(text);
                    compositor.DrawRegion(window->m_X + text->m_X, window->m_Y + text->m_Y, text->m_Width, text->m_Height);
                    
                    break;
                }
                
                case 65: // ACK
                break;
                
                default:
                    printf("Unrecognised event with id %u\n", event.id);
                break;
            }
        }
        
        compositor.DrawMouse(mouse);
        
        // Deal with events
        for (uint32_t i = 0; i < compositor.m_vWindows.Length(); ++i)
        {
            auto window = compositor.m_vWindows[i];
            auto updatedState = window->ShouldUpdate(mouse, compositor.m_screenWidth, compositor.m_screenHeight);
            
            if (updatedState.m_first)
                compositor.MoveWindow(window, updatedState.m_second.m_first, updatedState.m_second.m_second);
        }
    }
    
    return 0;
}
