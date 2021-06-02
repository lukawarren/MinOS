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
    m_pWidgets[0] = new Bar(m_Width, nBarHeight, m_X, m_Y);

    // Background panel
    m_pWidgets[1] = new Panel(m_Width, m_Height - nBarHeight, m_X, m_Y + nBarHeight);

    // Bar title
    m_pWidgets[2] = new Text("Terminal", m_X+nPadding, m_Y + nBarHeight/2 - CHAR_HEIGHT/2);

    // Bar button
    m_pWidgets[3] = new Panel
    (
        nButtonWidth,
        nButtonHeight,
        m_X + m_Width - nButtonWidth - nPadding,        // End of window minus button's width and padding
        m_Y + nBarHeight - nButtonHeight - nPadding/2   // End of bar minus button's height and half of padding
    );
    m_pWidgets[4] = new Text
    (
        "X",
        m_X + m_Width - nButtonWidth - nPadding + 6,    // End of window minus button's position, plus 6 padding for text
        m_Y + nBarHeight - nButtonHeight + 2            // (As above but with 2 padding for text)
    );

    // Dummy terminal
    const uint32_t terminalWidth = m_Width - 10;
    const uint32_t terminalHeight = m_Height - nBarHeight - 10;
    const uint32_t terminalX = m_X + 5;
    const uint32_t terminalY = m_Y + nBarHeight + 5;

    m_pWidgets[5] = new Panel(terminalWidth, terminalHeight, terminalX, terminalY, 0);
    m_pWidgets[6] = new Text("colonel@minos /home $", terminalX + nPadding, terminalY + CHAR_HEIGHT/2);
}

Graphics::Window::~Window()
{
    for (Widget* w : m_pWidgets)
            delete w;
}