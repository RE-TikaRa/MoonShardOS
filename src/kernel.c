#include <stdint.h>
#include "limine.h"
#include "logo.c"

/*
在启动MoonShardOS的时候，需要一个framebuffer。
*/
__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

static uint32_t make_color(
    struct limine_framebuffer *framebuffer,
    uint8_t r,
    uint8_t g,
    uint8_t b
)
{
    uint32_t color = 0;

    color |= ((uint32_t)r >> (8 - framebuffer->red_mask_size))
        << framebuffer->red_mask_shift;

    color |= ((uint32_t)g >> (8 - framebuffer->green_mask_size))
        << framebuffer->green_mask_shift;

    color |= ((uint32_t)b >> (8 - framebuffer->blue_mask_size))
        << framebuffer->blue_mask_shift;

    return color;
}

static void draw_image(
    struct limine_framebuffer *framebuffer
)
{
    uint32_t *pixels = framebuffer->address;
    uint64_t pitch = framebuffer->pitch / 4;

    uint64_t start_x =
        (framebuffer->width - IMAGE_WIDTH) / 2;

    uint64_t start_y =
        (framebuffer->height - IMAGE_HEIGHT) / 2;

    for (uint64_t y = 0; y < IMAGE_HEIGHT; y++)
    {
        for (uint64_t x = 0; x < IMAGE_WIDTH; x++)
        {
            uint32_t color =
                image[y * IMAGE_WIDTH + x];

            uint8_t a = (color >> 24) & 0xff;
            uint8_t r = (color >> 16) & 0xff;
            uint8_t g = (color >> 8) & 0xff;
            uint8_t b = color & 0xff;

            if (a == 0)
            {
                continue;
            }

            uint64_t index =
                (start_y + y) * pitch +
                start_x + x;

            uint32_t old = pixels[index];

            uint8_t old_r =
                (old >> framebuffer->red_mask_shift) &
                ((1 << framebuffer->red_mask_size) - 1);

            uint8_t old_g =
                (old >> framebuffer->green_mask_shift) &
                ((1 << framebuffer->green_mask_size) - 1);

            uint8_t old_b =
                (old >> framebuffer->blue_mask_shift) &
                ((1 << framebuffer->blue_mask_size) - 1);

            old_r =
                (old_r * 255) /
                ((1 << framebuffer->red_mask_size) - 1);

            old_g =
                (old_g * 255) /
                ((1 << framebuffer->green_mask_size) - 1);

            old_b =
                (old_b * 255) /
                ((1 << framebuffer->blue_mask_size) - 1);

            uint8_t final_r =
                (r * a + old_r * (255 - a)) / 255;

            uint8_t final_g =
                (g * a + old_g * (255 - a)) / 255;

            uint8_t final_b =
                (b * a + old_b * (255 - a)) / 255;

            pixels[index] =
                make_color(
                    framebuffer,
                    final_r,
                    final_g,
                    final_b
                );
        }
    }
}

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
};

static void draw_char(
    struct limine_framebuffer *framebuffer,
    uint64_t x,
    uint64_t y,
    uint32_t color,
    const uint8_t *glyph
)
{
    uint32_t *pixels = framebuffer->address;
    uint64_t pitch = framebuffer->pitch / 4;

    for (uint64_t py = 0; py < 8; py++)
    {
        for (uint64_t px = 0; px < 8; px++)
        {
            if (glyph[py] & (0x80 >> px))
            {
                pixels[
                    (y + py) * pitch +
                    x + px
                ] = color;
            }
        }
    }
}

static void draw_string(
    struct limine_framebuffer *framebuffer,
    uint64_t x,
    uint64_t y,
    uint32_t color,
    const uint8_t *font_data,
    const char *text
)
{
    while (*text)
    {
        const uint8_t *glyph = font_data;

        switch (*text)
        {
            case 'M': glyph += 0; break;
            case 'O': glyph += 8; break;
            case 'S': glyph += 16; break;
            case 'H': glyph += 24; break;
            case 'A': glyph += 32; break;
            case 'R': glyph += 40; break;
            case 'D': glyph += 48; break;
            case ' ': glyph += 56; break;
            case 'N': glyph += 64; break;
            default:
                text++;
                continue;
        }

        draw_char(
            framebuffer,
            x,
            y,
            color,
            glyph
        );

        x += 10;
        text++;
    }
}

void kmain(void)
{
    struct limine_framebuffer_response *response =
        framebuffer_request.response;

    /*
    判断是否存在framebuffer。
    */
    if (response == 0 ||
        response->framebuffer_count == 0)
    {
        for (;;)
        {
            __asm__ volatile("hlt");
        }
    }

    struct limine_framebuffer *framebuffer =
        response->framebuffers[0];

    uint32_t *pixels = framebuffer->address;
    uint64_t pitch = framebuffer->pitch / 4;

    /*
    羊皮纸背景：
    RGB = E8 D7 AD
    */
    uint32_t background =
        make_color(
            framebuffer,
            0xE8,
            0xD7,
            0xAD
        );

    for (uint64_t y = 0;
         y < framebuffer->height;
         y++)
    {
        for (uint64_t x = 0;
             x < framebuffer->width;
             x++)
        {
            pixels[y * pitch + x] = background;
        }
    }

    /*
    绘制LOGO。
    */
    draw_image(framebuffer);

    /*
    墨水蓝：
    RGB = 17 2A 46
    */
    uint32_t text_color =
        make_color(
            framebuffer,
            0x17,
            0x2A,
            0x46
        );

    draw_string(
        framebuffer,
        framebuffer->width / 2 - 45,
        framebuffer->height / 2 + 150,
        text_color,
        font,
        "MOON SHARD"
    );

    for (;;)
    {
        __asm__ volatile("hlt");
    }
}