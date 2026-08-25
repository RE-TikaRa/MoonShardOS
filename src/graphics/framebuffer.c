#include "framebuffer.h"

static struct limine_framebuffer *current_framebuffer;

void framebuffer_init(
    struct limine_framebuffer *framebuffer
)
{
    current_framebuffer = framebuffer;
}

struct limine_framebuffer *framebuffer_get(void)
{
    return current_framebuffer;
}

uint32_t framebuffer_make_color(
    uint8_t r,
    uint8_t g,
    uint8_t b
)
{
    uint32_t color = 0;

    color |= ((uint32_t)r >>
        (8 - current_framebuffer->red_mask_size))
        << current_framebuffer->red_mask_shift;

    color |= ((uint32_t)g >>
        (8 - current_framebuffer->green_mask_size))
        << current_framebuffer->green_mask_shift;

    color |= ((uint32_t)b >>
        (8 - current_framebuffer->blue_mask_size))
        << current_framebuffer->blue_mask_shift;

    return color;
}

void framebuffer_clear(
    uint32_t color
)
{
    uint32_t *pixels = current_framebuffer->address;
    uint64_t pitch = current_framebuffer->pitch / 4;

    for (uint64_t y = 0;
         y < current_framebuffer->height;
         y++)
    {
        for (uint64_t x = 0;
             x < current_framebuffer->width;
             x++)
        {
            pixels[y * pitch + x] = color;
        }
    }
}

void framebuffer_put_pixel(
    uint64_t x,
    uint64_t y,
    uint32_t color
)
{
    if (x >= current_framebuffer->width ||
        y >= current_framebuffer->height)
    {
        return;
    }

    uint32_t *pixels = current_framebuffer->address;
    uint64_t pitch = current_framebuffer->pitch / 4;

    pixels[y * pitch + x] = color;
}

uint32_t framebuffer_get_pixel(
    uint64_t x,
    uint64_t y
)
{
    if (x >= current_framebuffer->width ||
        y >= current_framebuffer->height)
    {
        return 0;
    }

    uint32_t *pixels = current_framebuffer->address;
    uint64_t pitch = current_framebuffer->pitch / 4;

    return pixels[y * pitch + x];
}