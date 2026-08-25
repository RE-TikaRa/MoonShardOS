/* Limine 从这里找到内核入口；kmain 不依赖 C 运行时启动代码。 */
ENTRY(kmain)

SECTIONS
{
    /*
     * 内核链接到高半部虚拟地址。Limine 负责建立启动所需的页表，
     * 因此内核中的符号和函数地址从这个基址开始布局。
     */
    . = 0xffffffff80000000;

    /* 代码段按页对齐，避免不同段共享同一页的权限边界。 */
    .text : ALIGN(4K)
    {
        *(.text*)
    }

    /* 只读常量，包括字体表和编译进来的图片像素。 */
    .rodata : ALIGN(4K)
    {
        *(.rodata*)
    }

    /* 可写的全局和静态数据。 */
    .data : ALIGN(4K)
    {
        *(.data*)
    }

    /* 未初始化数据和 COMMON 符号，启动时由加载环境清零。 */
    .bss : ALIGN(4K)
    {
        *(COMMON)
        *(.bss*)
    }

    /*
     * Limine 请求结构必须独立集中保存。KEEP 防止链接器丢弃看似未引用的
     * 请求，两个符号为启动器提供请求区间的边界。
     */
    .requests : ALIGN(8)
    {
        __requests_start = .;
        KEEP(*(.requests))
        __requests_end = .;
    }
}
