#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include "../limine.h"

void framebuffer_init(
    struct limine_framebuffer *framebuffer
);

struct limine_framebuffer *framebuffer_get(void);

uint32_t framebuffer_make_color(
    uint8_t r,
    uint8_t g,
    uint8_t b
);

void framebuffer_clear(
    uint32_t color
);

void framebuffer_put_pixel(
    uint64_t x,
    uint64_t y,
    uint32_t color
);

uint32_t framebuffer_get_pixel(
    uint64_t x,
    uint64_t y
);

#endif