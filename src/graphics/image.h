#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

void image_draw(
    uint64_t x,
    uint64_t y,
    uint64_t width,
    uint64_t height,
    const uint32_t *image
);

#endif