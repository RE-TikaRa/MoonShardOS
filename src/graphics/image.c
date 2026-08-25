#include "image.h"
#include "framebuffer.h"

void image_draw(
    uint64_t x,
    uint64_t y,
    uint64_t width,
    uint64_t height,
    const uint32_t *image
)
{
    struct limine_framebuffer *framebuffer =
        framebuffer_get();

    /* image 数组是按 y 行、x 列排列的连续像素。 */
    for (uint64_t iy = 0;
         iy < height;
         iy++)
    {
        for (uint64_t ix = 0;
             ix < width;
             ix++)
        {
            uint64_t target_x = x + ix;
            uint64_t target_y = y + iy;

            /* 图像可以部分位于屏幕外，越界像素不参与绘制。 */
            if (target_x >= framebuffer->width ||
                target_y >= framebuffer->height)
            {
                continue;
            }

            uint32_t color =
                image[iy * width + ix];

            uint8_t a = (color >> 24) & 0xFF;
            uint8_t r = (color >> 16) & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = color & 0xFF;

            /* Alpha 为 0 表示完全透明，保留背景像素。 */
            if (a == 0)
            {
                continue;
            }

            uint32_t old =
                framebuffer_get_pixel(
                    target_x,
                    target_y
                );

            /*
             * framebuffer 中的颜色分量是压缩后的 mask 值，
             * 先提取并恢复到 0~255，才能和图像的 8 位分量使用同一套
             * Alpha 公式计算。
             */
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

            /* 混合结果重新编码成 framebuffer 的原生像素格式。 */
            framebuffer_put_pixel(
                target_x,
                target_y,
                framebuffer_make_color(
                    final_r,
                    final_g,
                    final_b
                )
            );
        }
    }
}
