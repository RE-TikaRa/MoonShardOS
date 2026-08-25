#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <stdint.h>
#include "../limine.h"

/*
 * Limine framebuffer 的最小访问接口。
 *
 * 模块保存一个当前 framebuffer 指针；因此必须先调用
 * framebuffer_init，再使用其他函数。颜色值是按照当前硬件
 * RGB mask 编码的 32 位原生像素值，而不是固定的 AARRGGBB。
 */

/* 注册 Limine 返回的 framebuffer 描述符。 */
void framebuffer_init(
    struct limine_framebuffer *framebuffer
);

/* 返回 framebuffer_init 注册的共享描述符。 */
struct limine_framebuffer *framebuffer_get(void);

/* 把三个 8 位 RGB 分量转换为 framebuffer 的原生像素格式。 */
uint32_t framebuffer_make_color(
    uint8_t r,
    uint8_t g,
    uint8_t b
);

/* 用一个原生像素值填满整个 framebuffer。 */
void framebuffer_clear(
    uint32_t color
);

/* 写入单个像素；越过 framebuffer 边界的坐标会被忽略。 */
void framebuffer_put_pixel(
    uint64_t x,
    uint64_t y,
    uint32_t color
);

/* 读取单个像素；越过边界时返回 0。 */
uint32_t framebuffer_get_pixel(
    uint64_t x,
    uint64_t y
);

#endif
