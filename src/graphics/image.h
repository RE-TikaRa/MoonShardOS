#ifndef IMAGE_H
#define IMAGE_H

#include <stdint.h>

/*
 * 在当前 framebuffer 上绘制一块静态 RGBA 图像。
 * image 按行优先保存像素，每个元素为 0xAARRGGBB；x 和 y 指向
 * 图像左上角。绘制过程会裁剪屏幕外像素，并把非完全不透明像素
 * 与 framebuffer 中原有颜色混合。
 */
void image_draw(
    uint64_t x,
    uint64_t y,
    uint64_t width,
    uint64_t height,
    const uint32_t *image
);

#endif
