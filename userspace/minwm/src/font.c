#include "font.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

#define SSFN_IMPLEMENTATION
#include <ssfn.h>

#define CHARS_BEGIN 33 // "!"
#define CHARS_END 126 // "~"
#define CHARS (CHARS_BEGIN - CHARS_END)
#define SIZE 64

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
    void* contents = malloc(size);
    fread(contents, size, 1, file);
    fclose(file);

    // Load and free
    printf("ssfn_load...\n");
    ssfn_load(&ctx, contents);
    printf("done\n");
    ssfn_select(&ctx, SSFN_FAMILY_ANY, NULL, SSFN_STYLE_REGULAR, SIZE);
    free(contents);

    // Pre-render each glyph
    glyphs = malloc(sizeof(uint8_t*) * (size_t)CHARS);
    for (int i = CHARS_BEGIN; i < CHARS_END; ++i)
    {
        char string[2] =  { (char)i, '\0' };
        glyphs[i] = ssfn_text(&ctx, string, 0xffffffff);
        printf("built char %s\n", string);
    }
}

void free_font()
{
    for (int i = 0; i < CHARS; ++i) free(glyphs[i]);
    free(glyphs);
    ssfn_free(&ctx);
}

int main()
{
    init_font("Gidole-Regular.sfn");
    printf("I worked\n");

    volatile int hang = 1;
    while(hang) { hang = 1; }
    return 0;
}
