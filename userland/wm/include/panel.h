#pragma once
#ifndef PANEL_H
#define PANEL_H

#include <stdint.h>
#include <stddef.h>

#include "widget.h"

namespace Graphics
{
    // Colours
    constexpr uint32_t cPanelBackground    = GetColour(212, 208, 200);
    constexpr uint32_t cPanelLeadingEdge   = GetColour(128, 128, 128);
    constexpr uint32_t cPanelTrim          = GetColour(255, 255, 255);

    class Panel: public Widget
    {
    public:
        Panel() {}

        Panel(const unsigned int width, const unsigned int height, const unsigned int x, const unsigned int y, const uint32_t colour = cPanelBackground);

        ~Panel() {}

        uint32_t GetPixel(const uint32_t screenX, const uint32_t screenY) const override;
    
    private:
        uint32_t m_cPanelColour;
    };
}

#endif