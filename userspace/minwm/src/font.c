#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define SSFN_IMPLEMENTATION
#include <ssfn.h>

#define CHARS 256
#define SIZE 64

ssfn_buf_t** glyphs;

void init_font(const char* path)
{
    // Open file and get size
    FILE* file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read into buffer
    void* contents = malloc(size);
    fread(contents, size, 1, file);
    fclose(file);

    // Load and free
    ssfn_t ctx = { 0 };
    ssfn_load(&ctx, contents);
    ssfn_select(&ctx, SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR, SIZE);
    free(contents);

    // Pre-render each glyph
    glyphs = malloc(sizeof(uint8_t*) * CHARS);
    for (int i = 0; i < CHARS; ++i)
    {
        const char c = i;
        glyphs[i] = ssfn_text(&ctx, &c, 0xffffffff);
    }
}

void free_font()
{
    for (int i = 0; i < CHARS; ++i) free(glyphs[i]);
    free(glyphs);
}

int main()
{
    init_font("bob");

    volatile int hang = 1;
    while(hang) { printf("test\n"); hang = 1; }
    return 0;
}