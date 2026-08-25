#include "console.h"
#include "../graphics/framebuffer.h"
#include "../graphics/font.h"

/*
 * Console 光标使用像素坐标，而不是字符行列。
 * console_x / console_y 始终指向下一个字形左上角。
 */
static uint64_t console_x;
static uint64_t console_y;

/* 已经按 framebuffer mask 编码的当前文字颜色。 */
static uint32_t console_color;

void console_init(void)
{
    /* 初始光标位于屏幕左上角。 */
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
    /* Console 背景与启动画面保持一致。 */
    uint32_t background =
        framebuffer_make_color(
            0xE8,
            0xD7,
            0xAD
        );

    framebuffer_clear(background);

    /* 清屏同时丢弃旧的光标位置，下一字符从左上角开始。 */
    console_x = 0;
    console_y = 0;
}

void console_set_color(uint32_t color)
{
    /* 颜色由调用者提前通过 framebuffer_make_color 转换。 */
    console_color = color;
}

void console_putchar(char character)
{
    struct limine_framebuffer *framebuffer =
        framebuffer_get();

    /* 换行只改变行，不绘制字形。行高额外保留一个像素。 */
    if (character == '\n')
    {
        console_x = 0;
        console_y += FONT_HEIGHT + 1;
        return;
    }

    /* 回车回到当前行行首，保持 y 不变。 */
    if (character == '\r')
    {
        console_x = 0;
        return;
    }

    /* 普通字符由字体层绘制，Console 只负责光标推进。 */
    font_draw_char(
        console_x,
        console_y,
        console_color,
        character
    );

    console_x += FONT_WIDTH + 1;

    /* 当前字形放不下时，先换到下一行再处理后续字符。 */
    if (console_x + FONT_WIDTH > framebuffer->width)
    {
        console_x = 0;
        console_y += FONT_HEIGHT + 1;
    }

    /*
     * 没有滚屏缓冲区时，底部溢出采用清屏回到左上角的策略。
     * 这样不会把字形写到 framebuffer 之外。
     */
    if (console_y + FONT_HEIGHT > framebuffer->height)
    {
        console_clear();
    }
}

void console_write(const char *text)
{
    /* 逐字符转交 console_putchar，保留换行和回车语义。 */
    while (*text)
    {
        console_putchar(*text);
        text++;
    }
}
