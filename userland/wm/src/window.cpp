#include "window.h"
#include "panel.h"
#include "bar.h"
#include "text.h"

#define WIDTH 1024
#define HEIGHT 768

constexpr uint32_t nBarHeight = 20;
constexpr uint32_t nButtonWidth = 20;
constexpr uint32_t nButtonHeight = 16;
constexpr uint32_t nPadding = 5;

Graphics::Window::Window(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, char const* title):
    m_Width(width), m_Height(height), m_X(x), m_Y(y), m_sTitle(title)
{
    // Bar
    m_vWidgets.Push(new Bar());

    // Background panel
    m_vWidgets.Push(new Panel());

    // Bar title
    m_vWidgets.Push(new Text(title));

    // Bar button
    m_vWidgets.Push(new Panel());
    m_vWidgets.Push(new Text("X"));
    
    Redraw();
}

void Graphics::Window::Redraw()
{
    // Bar
    m_vWidgets[0]->Redraw(m_Width, nBarHeight, m_X, m_Y);

    // Background panel
    m_vWidgets[1]->Redraw(m_Width, m_Height - nBarHeight, m_X, m_Y + nBarHeight);

    // Bar title
    m_vWidgets[2]->Redraw(m_X+nPadding, m_Y + nBarHeight/2 - CHAR_HEIGHT/2);

    // Bar button
    m_vWidgets[3]->Redraw
    (
        nButtonWidth,
        nButtonHeight,
        m_X + m_Width - nButtonWidth - nPadding,        // End of window minus button's width and padding
        m_Y + nBarHeight - nButtonHeight - nPadding/2   // End of bar minus button's height and half of padding
    );
    m_vWidgets[4]->Redraw
    (
        m_X + m_Width - nButtonWidth - nPadding + 6,    // End of window minus button's position, plus 6 padding for text
        m_Y + nBarHeight - nButtonHeight + 2            // (As above but with 2 padding for text)
    );
}

Graphics::Window::~Window() {}
