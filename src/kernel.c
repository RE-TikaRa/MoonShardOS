#include <stdint.h>
#include "limine.h"
#include "graphics/framebuffer.h"
#include "graphics/image.h"
#include "graphics/font.h"
#include "logo.c"
#include "console/console.h"
#include "cpu/gdt.h"

/*
在启动MoonShardOS的时候，需要一个framebuffer。
*/
__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

void kmain(void)
{
    struct limine_framebuffer_response *response =
        framebuffer_request.response;

    /*
    判断是否存在framebuffer。
    */
    if (response == 0 ||
        response->framebuffer_count == 0)
    {
        for (;;)
        {
            __asm__ volatile("hlt");
        }
    }

    struct limine_framebuffer *framebuffer =
        response->framebuffers[0];

    gdt_init();

    framebuffer_init(framebuffer);

    /*
    羊皮纸背景：
    RGB = E8 D7 AD
    */
    uint32_t background =
        framebuffer_make_color(
            0xE8,
            0xD7,
            0xAD
        );

    framebuffer_clear(background);

    /*
    绘制LOGO。
    */
    image_draw(
        (framebuffer->width - IMAGE_WIDTH) / 2,
        (framebuffer->height - IMAGE_HEIGHT) / 2,
        IMAGE_WIDTH,
        IMAGE_HEIGHT,
        image
    );

    /*
    墨水蓝：
    RGB = 17 2A 46
    */
    uint32_t text_color =
        framebuffer_make_color(
            0x17,
            0x2A,
            0x46
        );

    console_init();

    console_set_color(text_color);

    console_write(
        "MoonShardOS\n"
        "Kernel initialized.\n"
        "Graphics initialized.\n"
    );



    for (;;)
    {
        __asm__ volatile("hlt");
    }
}