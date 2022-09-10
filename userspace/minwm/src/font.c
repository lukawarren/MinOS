#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define SSFN_IMPLEMENTATION
#include <ssfn.h>

#define CHARS 255
#define SIZE 64

void* font;
ssfn_buf_t** glyphs;
ssfn_t ctx = { 0 };

void init_font(const char* path)
{
    // Open file and get size
    FILE* file = fopen(path, "r");
    assert(file != NULL);
    fseek(file, 0, SEEK_END);
    size_t size = (size_t) ftell(file);
    fseek(file, 0, SEEK_SET);

    // Read into buffer
    font = malloc(size);
    fread(font, size, 1, file);
    fclose(file);

    // Load
    memset(&ctx, 0, sizeof(ssfn_t));
    ssfn_load(&ctx, font);
    ssfn_select(&ctx, SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR, SIZE);

    // Pre-render each glyph
    glyphs = malloc(sizeof(ssfn_buf_t*) * (size_t)CHARS);
    for (int i = 0; i < CHARS; ++i)
    {
        char string[2] =  { (char)i, '\0' };
        glyphs[i] = ssfn_text(&ctx, string, 0xffffffff);
    }

    printf("[minwm] Loaded font %s\n", path);
}

void free_font()
{
    for (int i = 0; i < CHARS; ++i) free(glyphs[i]);
    free(glyphs);
    free(font);
    ssfn_free(&ctx);
    printf("[minwm] Unloaded font\n");
}

int main()
{
    init_font("Gidole-Regular.sfn");
    free_font();

    volatile int hang = 1;
    while(hang) { hang = 1; }
    return 0;
}
