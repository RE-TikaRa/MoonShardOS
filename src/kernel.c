#include <stdint.h>
#include "limine.h"
#include "graphics/framebuffer.h"
#include "graphics/image.h"
#include "graphics/font.h"
#include "logo.c"
#include "console/console.h"
#include "cpu/gdt.h"
#include "cpu/idt.h"

/*
 * Limine 会在启动阶段扫描放入 .requests 段的请求结构，
 * 然后把 framebuffer 响应地址写回 request.response。
 *
 * 这个请求必须保留在最终内核镜像中，因此结构体同时标记为 used，
 * 并显式放入链接脚本收集的 .requests 段。
 */
__attribute__((used, section(".requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

void kmain(void)
{
    /*
     * 入口由 Limine 调用。这里不依赖任何 C 运行时初始化，
     * 所有内核状态都由各子系统自行建立。
     */
    struct limine_framebuffer_response *response =
        framebuffer_request.response;

    /*
     * 没有 framebuffer 时，图形 Console 和异常输出都无法工作。
     * 内核只能停在 hlt 循环中，避免继续解引用空响应指针。
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

    /*
     * 先加载内核自己的代码段和数据段。
     * GDT 初始化会刷新段寄存器以及 CS，后续 IDT 门也使用其中的
     * 内核代码段选择子 0x08。
     */
    gdt_init();

    /*
     * 图形模块只保存 Limine 提供的 framebuffer 描述符，
     * 之后的清屏、像素、图片和字体操作都通过这个共享状态访问它。
     */
    framebuffer_init(framebuffer);

    /*
     * 启动画面使用羊皮纸色背景。
     * framebuffer_make_color 会按照硬件实际提供的 RGB mask
     * 把普通的 8 位 RGB 分量转换为原生像素格式。
     */
    uint32_t background =
        framebuffer_make_color(
            0xE8,
            0xD7,
            0xAD
        );

    framebuffer_clear(background);

    /*
     * logo.c 是由 tools/png2c.py 从 PNG 生成的静态像素数组。
     * 以屏幕中心为锚点绘制，image_draw 会负责屏幕边界裁剪和 Alpha 混合。
     */
    image_draw(
        (framebuffer->width - IMAGE_WIDTH) / 2,
        (framebuffer->height - IMAGE_HEIGHT) / 2,
        IMAGE_WIDTH,
        IMAGE_HEIGHT,
        image
    );

    /*
     * Console 文字采用墨水蓝。颜色必须在 framebuffer 初始化之后转换，
     * 因为转换依赖 Limine 返回的 mask 位宽和位移。
     */
    uint32_t text_color =
        framebuffer_make_color(
            0x17,
            0x2A,
            0x46
        );

    /*
     * 异常处理函数会通过 Console 输出信息，所以 Console 必须先于 IDT
     * 完成初始化；否则异常发生时没有可用的文字输出路径。
     */
    console_init();

    console_set_color(text_color);

    idt_init();

    console_write(
        "MoonShardOS\n"
        "Kernel initialized.\n"
        "Graphics initialized.\n"
    );


    /*
     * 当前内核没有调度器或可运行任务。初始化完成后停机等待，
     * 让 CPU 不在空循环中持续消耗执行资源，同时保持已安装的 IDT 可用。
     */
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}
