#ifndef FONT_H
#define FONT_H

#include <stdint.h>

void font_draw_char(
    uint64_t x,
    uint64_t y,
    uint32_t color,
    char character
);

void font_draw_string(
    uint64_t x,
    uint64_t y,
    uint32_t color,
    const char *text
);

#endif