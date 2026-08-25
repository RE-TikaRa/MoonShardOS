#include "console.h"
#include "../graphics/framebuffer.h"
#include "../graphics/font.h"

static uint64_t console_x;
static uint64_t console_y;
static uint32_t console_color;

void console_init(void)
{
    console_x = 0;
    console_y = 0;

    console_color =
        framebuffer_make_color(
            0x17,
            0x2A,
            0x46
        );
}

void console_clear(void)
{
    uint32_t background =
        framebuffer_make_color(
            0xE8,
            0xD7,
            0xAD
        );

    framebuffer_clear(background);

    console_x = 0;
    console_y = 0;
}

void console_set_color(uint32_t color)
{
    console_color = color;
}

void console_putchar(char character)
{
    struct limine_framebuffer *framebuffer =
        framebuffer_get();

    if (character == '\n')
    {
        console_x = 0;
        console_y += FONT_HEIGHT + 1;
        return;
    }

    if (character == '\r')
    {
        console_x = 0;
        return;
    }

    font_draw_char(
        console_x,
        console_y,
        console_color,
        character
    );

    console_x += FONT_WIDTH + 1;

    if (console_x + FONT_WIDTH > framebuffer->width)
    {
        console_x = 0;
        console_y += FONT_HEIGHT + 1;
    }

    if (console_y + FONT_HEIGHT > framebuffer->height)
    {
        console_clear();
    }
}

void console_write(const char *text)
{
    while (*text)
    {
        console_putchar(*text);
        text++;
    }
}