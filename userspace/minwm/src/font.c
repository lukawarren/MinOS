#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <stddef.h>

#define SSFN_IMPLEMENTATION
#define _UINT64_T uint64_t
#include <ssfn.h>

#define FRAMEBUFFER_WIDTH 1280
#define FRAMEBUFFER_ADDRESS 0x30000000
#define FAST_DRAW 0

void* font;
ssfn_t ctx = { 0 };

void init_font(const char* path, int font_size)
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
    const int style = SSFN_STYLE_REGULAR | SSFN_STYLE_NOAA;
    memset(&ctx, 0, sizeof(ssfn_t));
    ssfn_load(&ctx, font);
    ssfn_select(&ctx, SSFN_FAMILY_ANY, NULL, style, font_size);
    printf("[minwm] Loaded font %s\n", path);
}

void draw_font(const char* message, uint32_t colour, unsigned int x, unsigned int y)
{
    ssfn_buf_t* buffer = ssfn_text(&ctx, message, colour);

    #if FAST_DRAW
        for (unsigned int text_y = 0; text_y < (unsigned int)buffer->h; ++text_y)
        {
            // Copy row
            uint8_t* out = (uint8_t*) FRAMEBUFFER_ADDRESS + 4 * ((y+text_y) * FRAMEBUFFER_WIDTH + x);
            uint8_t* in = buffer->ptr + text_y * buffer->p;
            memcpy(out, in, buffer->p);
        }
    #else
        for (unsigned int text_y = 0; text_y < (unsigned int)buffer->h; ++text_y)
        {
            for (unsigned int text_x = 0; text_x < (unsigned int)buffer->w; ++text_x)
            {
                uint8_t* out = (uint8_t*) FRAMEBUFFER_ADDRESS + 4 * ((y+text_y) * FRAMEBUFFER_WIDTH + x + text_x);
                uint8_t* in = buffer->ptr + text_y * buffer->p + text_x * 4;
                if (*in != 0)
                    memcpy(out, in, 4);
            }
        }
    #endif

    free(buffer->ptr);
    free(buffer);
}

void draw_font_centered(const char* message, uint32_t colour, unsigned int x, unsigned int y, unsigned int boundsX, unsigned int boundsY)
{
    // int ssfn_bbox(ssfn_t *ctx, const char *str, int *w, int *h, int *left, int *top);
    int width, height, left, top;
    ssfn_bbox(&ctx, message, &width, &height, &left, &top);
    draw_font(
        message,
        colour,
        x + boundsX / 2 - (unsigned int)width / 2,
        y + boundsY / 2 - (unsigned int)height / 2
    );
}

void free_font(void)
{
    free(font);
    ssfn_free(&ctx);
    printf("[minwm] Unloaded font\n");
}
