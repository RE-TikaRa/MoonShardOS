#include "font.h"
#include "framebuffer.h"

#define FONT_WIDTH 8
#define FONT_HEIGHT 8
#define FONT_SPACING 10

static const uint8_t font[] = {
    /* M */
    0x81, 0xC3, 0xA5, 0x99, 0x81, 0x81, 0x81, 0x81,

    /* O */
    0x3C, 0x66, 0xC3, 0xC3, 0xC3, 0xC3, 0x66, 0x3C,

    /* S */
    0x3C, 0x66, 0x60, 0x3C, 0x06, 0x06, 0x66, 0x3C,

    /* H */
    0xC3, 0xC3, 0xC3, 0xFF, 0xFF, 0xC3, 0xC3, 0xC3,

    /* A */
    0x18, 0x3C, 0x66, 0x66, 0xFF, 0xFF, 0x66, 0x66,

    /* R */
    0xFC, 0x66, 0x66, 0x7C, 0x6C, 0x66, 0x66, 0xE6,

    /* D */
    0xF8, 0x6C, 0x66, 0x66, 0x66, 0x66, 0x6C, 0xF8,

    /* space */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

    /* N */
    0xC3, 0xE3, 0xF3, 0xDB, 0xCF, 0xC7, 0xC3, 0xC3,

    /* ? */
    0x3C, 0x66, 0x06, 0x0C, 0x18, 0x00, 0x18, 0x18
};

static const uint8_t *font_get_glyph(
    char character
)
{
    switch (character)
    {
        case 'M': return font + 0;
        case 'O': return font + 8;
        case 'S': return font + 16;
        case 'H': return font + 24;
        case 'A': return font + 32;
        case 'R': return font + 40;
        case 'D': return font + 48;
        case ' ': return font + 56;
        case 'N': return font + 64;
        default: return font + 72;
    }
}

void font_draw_char(
    uint64_t x,
    uint64_t y,
    uint32_t color,
    char character
)
{
    const uint8_t *glyph =
        font_get_glyph(character);

    for (uint64_t py = 0;
         py < FONT_HEIGHT;
         py++)
    {
        for (uint64_t px = 0;
             px < FONT_WIDTH;
             px++)
        {
            if (glyph[py] & (0x80 >> px))
            {
                framebuffer_put_pixel(
                    x + px,
                    y + py,
                    color
                );
            }
        }
    }
}

void font_draw_string(
    uint64_t x,
    uint64_t y,
    uint32_t color,
    const char *text
)
{
    while (*text)
    {
        font_draw_char(
            x,
            y,
            color,
            *text
        );

        x += FONT_SPACING;
        text++;
    }
}