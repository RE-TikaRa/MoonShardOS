#include "framebuffer.h"

/*
 * Limine 描述符由启动器提供，模块只保存指针，不复制 framebuffer 元数据
 * 或像素内存。这个指针在内核运行期间保持有效。
 */
static struct limine_framebuffer *current_framebuffer;

void framebuffer_init(
    struct limine_framebuffer *framebuffer
)
{
    /* 后续所有图形操作都以这个描述符作为目标。 */
    current_framebuffer = framebuffer;
}

struct limine_framebuffer *framebuffer_get(void)
{
    /* 图形上层通过该函数读取尺寸、pitch 和 RGB mask。 */
    return current_framebuffer;
}

uint32_t framebuffer_make_color(
    uint8_t r,
    uint8_t g,
    uint8_t b
)
{
    /*
     * Limine 返回每个颜色分量在像素中的位宽和起始位。
     * 先把 8 位分量压缩到硬件支持的位宽，再移到对应位置，
     * 这样同一套绘图代码可以适配不同的 RGB 排列。
     */
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
    /*
     * pitch 是一行占用的字节数，不一定等于 width * 4。
     * 当前绘图路径按 32 位像素访问，因此换算成 uint32_t 元素数后，
     * 每一行使用 pitch 而不是 width 计算起始地址。
     */
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
    /*
     * 统一在底层拦截越界坐标，使字体和图片等上层绘制逻辑可以直接
     * 传入屏幕边缘附近的坐标，而不会写出 framebuffer。
     */
    if (x >= current_framebuffer->width ||
        y >= current_framebuffer->height)
    {
        return;
    }

    uint32_t *pixels = current_framebuffer->address;
    uint64_t pitch = current_framebuffer->pitch / 4;

    /* 一维像素数组的行首偏移由 pitch 决定，列偏移由 x 决定。 */
    pixels[y * pitch + x] = color;
}

uint32_t framebuffer_get_pixel(
    uint64_t x,
    uint64_t y
)
{
    /* 读取与写入使用完全相同的边界和 pitch 规则。 */
    if (x >= current_framebuffer->width ||
        y >= current_framebuffer->height)
    {
        return 0;
    }

    uint32_t *pixels = current_framebuffer->address;
    uint64_t pitch = current_framebuffer->pitch / 4;

    return pixels[y * pitch + x];
}
