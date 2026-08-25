#ifndef FONT_H
#define FONT_H

#include <stdint.h>

/* 每个字形占用 8x8 个像素，字符串绘制时每个字符前移 8 像素。 */
#define FONT_WIDTH 8
#define FONT_HEIGHT 8
#define FONT_SPACING 8

/* 在指定左上角绘制一个 ASCII 字符。 */
void font_draw_char(
    uint64_t x,
    uint64_t y,
    uint32_t color,
    char character
);

/* 从指定左上角开始按 FONT_SPACING 连续绘制字符串。 */
void font_draw_string(
    uint64_t x,
    uint64_t y,
    uint32_t color,
    const char *text
);

#endif
