#pragma once

#include <stdint.h>
#include <assert.h>

#define SSFN_IMPLEMENTATION
#include <ssfn.h>

class Font
{
public:
    Font(const char* path);
    ~Font();

    uint8_t* get_char(const char c) const;

private:
    SSFN::Font font;
    ssfn_buf_t** glyphs;
};
