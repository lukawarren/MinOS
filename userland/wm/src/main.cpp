#include <minlib.h>
#include "compositor.h"
#include "mouse.h"
#include "keyboard.h"
#include "events.h"
#include "panel.h"
#include "text.h"
#include "button.h"
#include "textArea.h"

int main()
{
    Graphics::Compositor compositor;
    Input::Mouse mouse = Input::Mouse(compositor.m_screenWidth / 2, compositor.m_screenHeight / 2);
    Input::Keyboard keyboard;
    
    loadprogram("launcher/launcher.bin");
    loadprogram("notepad/notepad.bin");
    loadprogram("bar/bar.bin");
    
    Graphics::Window* pActiveWindow = nullptr;
    while (1)
    {
        // Process all events
        Pair<bool, Message> message;
        while ((message = Event<>::GetMessage()).m_first)
        {
            Event<>::Ack(message);
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
                        createWindowEvent->width,
                        createWindowEvent->height,
                        createWindowEvent->x,
                        createWindowEvent->y,
                        createWindowEvent->sName,
                        pid,
                        createWindowEvent->bDecorated
                    );
                    
                    compositor.m_vWindows.Push(window);
                    compositor.DrawRegion(window->m_X, window->m_Y, window->m_Width, window->m_Height);
                    pActiveWindow = window;
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
                    if (window == pActiveWindow) pActiveWindow = nullptr;
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
                
                case BUTTON_CREATE:
                {
                    auto window = compositor.GetWindowForPID(pid);
                    if (window == nullptr) break;
                    
                    eButtonCreate* createButtonEvent = (eButtonCreate*) event.data;

                    auto button = new Graphics::Button
                    (
                        createButtonEvent->text,
                        createButtonEvent->x,
                        createButtonEvent->y,
                        createButtonEvent->width,
                        createButtonEvent->height
                    );
                    
                    window->AddWidget(button);
                    compositor.DrawRegion(window->m_X + button->m_X, window->m_Y + button->m_Y, button->m_Width, button->m_Height);
                    break;
                }
                
                case TEXT_AREA_CREATE:
                {
                    auto window = compositor.GetWindowForPID(pid);
                    if (window == nullptr) break;
                    
                    eTextAreaCreate* createTextAreaEvent = (eTextAreaCreate*) event.data;

                    auto textArea = new Graphics::TextArea
                    (
                        createTextAreaEvent->x,
                        createTextAreaEvent->y,
                        createTextAreaEvent->width,
                        createTextAreaEvent->height,
                        createTextAreaEvent->colour
                    );
                    
                    window->AddWidget(textArea);
                    compositor.DrawRegion(window->m_X + textArea->m_X, window->m_Y + textArea->m_Y, textArea->m_Width, textArea->m_Height);
                    break;
                }
                
                case TEXT_AREA_ADD_ROW:
                {
                    auto window = compositor.GetWindowForPID(pid);
                    if (window == nullptr) break;
                    
                    eTextAreaAddRow* createRowEvent = (eTextAreaAddRow*) event.data;
                    auto widget = (Graphics::TextArea*)window->GetWidgetFromUserIndex(createRowEvent->index);
                    widget->AddRow(createRowEvent->text);
                    if (createRowEvent->bRefresh)
                    {
                        widget->Render();
                        compositor.DrawRegion(window->m_X + widget->m_X, window->m_Y + widget->m_Y, widget->m_Width, widget->m_Height);
                    }
                    break;
                }
                
                case TEXT_AREA_SET_ROW:
                {
                    auto window = compositor.GetWindowForPID(pid);
                    if (window == nullptr) break;
                    
                    eTextAreaSetRow* setRowEvent = (eTextAreaSetRow*) event.data;
                    auto widget = (Graphics::TextArea*)window->GetWidgetFromUserIndex(setRowEvent->index);
                    widget->SetRow(setRowEvent->row, setRowEvent->text);
                    widget->Render();
                    compositor.DrawRegion(window->m_X + widget->m_X, window->m_Y + widget->m_Y, widget->m_Width, widget->m_Height);
                    break;
                }
                
                case TEXT_SET:
                {
                    auto window = compositor.GetWindowForPID(pid);
                    if (window == nullptr) break;
                    
                    eTextSet* setTextEvent = (eTextSet*) event.data;
                    auto widget = (Graphics::Text*)window->GetWidgetFromUserIndex(setTextEvent->index);
                    widget->SetText(setTextEvent->text);
                    widget->Render();
                    compositor.DrawRegion(window->m_X + widget->m_X, window->m_Y + widget->m_Y, widget->m_Width, widget->m_Height);
                    break;
                }

                case PANEL_SET_COLOUR:
                {
                    auto window = compositor.GetWindowForPID(pid);
                    if (window == nullptr) break;
                    
                    ePanelColour* panelColourEvent = (ePanelColour*) event.data;

                    // TODO: Sanity check
                    auto panel = ((Graphics::Panel*) window->GetWidgetFromUserIndex(panelColourEvent->index));
                    panel->SetColour(panelColourEvent->colour);
                    panel->Render();
                    compositor.DrawRegion(window->m_X + panel->m_X, window->m_Y + panel->m_Y, panel->m_Width, panel->m_Height);
                    
                    break;
                }
                
                default:
                    printf("[Wm] Unrecognised event with id %u\n", event.id);
                    exit(-1);
                break;
            }
        }
        
        keyboard.Poll();
        compositor.UpdateAndDrawMouse(mouse);
        
        // De-active active window when mouse lifted (if any)
        if (pActiveWindow && mouse.m_sState.bLeftButton == false && pActiveWindow->IsHoveredOver(mouse) == false)
        {
            auto region = pActiveWindow->Unhighlight();
            compositor.DrawRegion(pActiveWindow->m_X, pActiveWindow->m_Y, region.m_first, region.m_second);
            pActiveWindow = nullptr;
        }

        // Find active window if mouse not down (if any)
        if (!pActiveWindow && mouse.m_sState.bLeftButton == false)
        {
            for (uint32_t i = 0; i < compositor.m_vWindows.Length() && !pActiveWindow; ++i)
            {
                const auto window = compositor.m_vWindows[i];
                if (window->IsHoveredOver(mouse))
                {
                    pActiveWindow = window;
                    auto region = pActiveWindow->Highlight();
                    compositor.DrawRegion(pActiveWindow->m_X, pActiveWindow->m_Y, region.m_first, region.m_second);
                }
            }
        }
        
        // Deal with events
        if (pActiveWindow != nullptr)
        {
            auto updatedState = pActiveWindow->ShouldUpdate(mouse, compositor.m_screenWidth, compositor.m_screenHeight, keyboard);
            
            if (updatedState.m_first)
                compositor.MoveWindow(pActiveWindow, updatedState.m_second.m_first, updatedState.m_second.m_second);
        }
    }
    
    return 0;
}
