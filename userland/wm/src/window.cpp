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
}

Pair<bool, Pair<uint32_t, uint32_t>> Graphics::Window::ShouldUpdate(const Input::Mouse& mouse, const uint32_t screenWidth, const uint32_t screenHeight)
{
    // If mouse is clicked and over quit, send quit event
    if (!m_bSentExitRequest && mouse.m_sState.bLeftButton &&
        m_vWidgets[3]->IsPixelSet(mouse.m_sState.x - m_X, mouse.m_sState.y - m_Y) &&
        m_vWidgets[3]->IsRowSet(mouse.m_sState.y - m_Y))
    {
        eExit { m_PID };
        m_bSentExitRequest = true;
    }
    
    // If mouse is clicked and over bar, move
    else if (mouse.m_sState.bLeftButton &&
        m_vWidgets[0]->IsPixelSet(mouse.m_sState.x - m_X, mouse.m_sState.y - m_Y) &&
        m_vWidgets[0]->IsRowSet(mouse.m_sState.y - m_Y))
    {
        m_bDragged = true;
    }

    else if (mouse.m_sState.bLeftButton == false) m_bDragged = false;
    
    if (m_bDragged)
    {
        return
        {
            true,
            {
                MIN((unsigned int) (mouse.m_sState.x), screenWidth - m_Width),
                MIN((unsigned int) (mouse.m_sState.y), screenHeight - m_Height)
            }
        };
    }
    
    return { false, { 0, 0 }};
}

void Graphics::Window::AddWidget(Widget* pWidget)
{
    pWidget->m_Y += nBarHeight;
    m_vWidgets.Push(pWidget);
    pWidget->Render();
}

Graphics::Window::~Window() {}
