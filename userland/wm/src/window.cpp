#include "window.h"
#include "panel.h"
#include "bar.h"
#include "text.h"
#include "events.h"

#define WIDTH 1024
#define HEIGHT 768

constexpr uint32_t nBarHeight = 20;
constexpr uint32_t nButtonWidth = 20;
constexpr uint32_t nButtonHeight = 16;
constexpr uint32_t nPadding = 5;

Graphics::Window::Window(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, char const* title, const uint32_t pid):
    m_Width(width), m_Height(height), m_X(x), m_Y(y), m_sTitle(title), m_PID(pid)
{
    // Bar
    m_vWidgets.Push(new Bar(m_Width, nBarHeight, 0, 0));

    // Background panel
    m_vWidgets.Push(new Panel(m_Width, m_Height, 0, nBarHeight));

    // Bar title
    m_vWidgets.Push(new Text(title, nPadding, nBarHeight/2 - CHAR_HEIGHT/2));

    // Bar button
    m_vWidgets.Push(new Panel
    (
        nButtonWidth,
        nButtonHeight,
        m_Width - nButtonWidth - nPadding,        // End of window minus button's width and padding
        nBarHeight - nButtonHeight - nPadding/2   // End of bar minus button's height and half of padding
    ));
    m_vWidgets.Push(new Text
    (
        "X",
        m_Width - nButtonWidth - nPadding + 6,    // End of window minus button's position, plus 6 padding for text
        nBarHeight - nButtonHeight + 2            // (As above but with 2 padding for text)
    ));

    m_bDragged = false;
    m_bSentExitRequest = false;

    for (size_t i = 0; i < m_vWidgets.Length(); ++i)
        m_vWidgets[i]->Render();

    m_Height += nBarHeight;
    m_nBaseWidgets = m_vWidgets.Length();
}

bool Graphics::Window::IsHoveredOver(const Input::Mouse& mouse) const
{
    return
        (unsigned int) mouse.m_sState.x >= m_X &&
        (unsigned int) mouse.m_sState.y >= m_Y &&
        (unsigned int) mouse.m_sState.x < m_X + m_Width &&
        (unsigned int) mouse.m_sState.y < m_Y + m_Height;
}

Pair<bool, Pair<uint32_t, uint32_t>> Graphics::Window::ShouldUpdate(const Input::Mouse& mouse, const uint32_t screenWidth, const uint32_t screenHeight)
{
    // If mouse is clicked and over quit (and we're not getting dragged!), send quit event
    if (!m_bSentExitRequest && mouse.m_sState.bLeftButton && !m_bDragged &&
        m_vWidgets[3]->IsPixelSet(mouse.m_sState.x - m_X, mouse.m_sState.y - m_Y) &&
        m_vWidgets[3]->IsRowSet(mouse.m_sState.y - m_Y))
    {
        eExit { m_PID };
        m_bSentExitRequest = true;
    }
    
    // If mouse is clicked and over bar, move
    else if (!m_bSentExitRequest && 
    mouse.m_sState.bLeftButton &&
        m_vWidgets[0]->IsPixelSet(mouse.m_sState.x - m_X, mouse.m_sState.y - m_Y) &&
        m_vWidgets[0]->IsRowSet(mouse.m_sState.y - m_Y)
        && !m_bDragged)
    {
        m_bDragged = true;
        m_dragOffsetX = m_X - mouse.m_sState.x;
        m_dragOffsetY = m_Y - mouse.m_sState.y;
    }

    else if (mouse.m_sState.bLeftButton == false) m_bDragged = false;

    // For every button, dispatch click events too
    bool bRedrawWindow = false;
    if (!m_bDragged)
    {
        for (size_t i = 0; i < m_vWidgets.Length(); ++i)
        {
            auto update = m_vWidgets[i]->ShouldUpdate(mouse, m_X, m_Y);
            if (update.m_first)
            {
                // Redraw widget after dealing with events
                m_vWidgets[i]->Render();
                bRedrawWindow = true;
                
                // Dispatch event if need be
                if (update.m_second) eWidgetUpdate { m_PID, i - m_nBaseWidgets};
            }
        }
    }
    
    if (m_bDragged)
    { 
        return // return coords to move to
        {
            true,
            {
                MIN((unsigned int) MAX(mouse.m_sState.x + m_dragOffsetX, 0), screenWidth - m_Width),
                MIN((unsigned int) MAX(mouse.m_sState.y + m_dragOffsetY, 0), screenHeight - m_Height)
            }
        };
    }
    
    else if (bRedrawWindow)
    {
        return { true, { m_X, m_Y } };
    }
    
    return { false, { 0, 0 }};
}

void Graphics::Window::AddWidget(Widget* pWidget)
{
    pWidget->m_Y += nBarHeight;
    m_vWidgets.Push(pWidget);
    pWidget->Render();
}

Graphics::Widget* Graphics::Window::GetWidgetFromUserIndex(const uint32_t index)
{
    return m_vWidgets[index + m_nBaseWidgets];
}

Pair<uint32_t, uint32_t> Graphics::Window::Highlight()
{
    ((Bar*)m_vWidgets[0])->SetColour(0xffdddddd);
    m_vWidgets[0]->Render();
    return { m_vWidgets[0]->m_Width, m_vWidgets[0]->m_Height };
}

Pair<uint32_t, uint32_t> Graphics::Window::Unhighlight()
{
    ((Bar*)m_vWidgets[0])->SetColour(0xffffffff);
    m_vWidgets[0]->Render();
    return { m_vWidgets[0]->m_Width, m_vWidgets[0]->m_Height };
}

Graphics::Window::~Window() {}
