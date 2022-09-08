#include "font.h"
#include <stdio.h>
#include <stdlib.h>

constexpr int chars = 256;

Font::Font(const char* path)
{
    // Open file and get size
    FILE* file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    auto size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read into buffer
    void* contents = malloc(size);
    fread(contents, size, 1, file);
    fclose(file);

    // Load and free
    font.Load(contents);
    font.Select(SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR, CHAR_WIDTH);
    free(contents);

    // Pre-render each glyph
    glyphs = (ssfn_buf_t**) malloc(sizeof(uint8_t*) * chars);
    for (auto i = 0; i < chars; ++i)
    {
        const char c = i;
        glyphs[i] = font.Text(&c, 0xffffffff);
    }
}

uint8_t* Font::get_char(const char c) const
{
    return (uint8_t*) glyphs[c];
}

Font::~Font()
{
    for (auto i = 0; i < chars; ++i) free(glyphs[i]);
    free(glyphs);
}
