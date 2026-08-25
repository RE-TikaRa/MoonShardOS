#include "idt.h"

#include <stdint.h>

/*
 * IDT 负责把中断向量号映射到处理函数。
 * 本文件只安装 CPU 保留的 0~31 号异常，硬件 IRQ 和软件中断仍未占用
 * 其余向量，数组的静态存储期初始化会把它们保持为全零描述符。
 */
/*
 * x86-64 IDT 门描述符。
 *
 * 一个 IDT entry 占 16 字节。
 *
 * 处理函数地址会被拆成：
 *
 * offset_low
 * offset_middle
 * offset_high
 *
 * CPU 发生异常时会重新组合成完整的 64 位地址。
 */
struct idt_entry
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_middle;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

/*
 * IDTR 使用的结构。
 *
 * limit = IDT 大小 - 1
 * base  = IDT 起始地址
 */
struct idt_pointer
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

/*
 * 一个 ISR 的函数指针类型。
 */
typedef void (*isr_handler_t)(void);

/*
 * CPU 异常入口。
 *
 * 这些函数由 isr.S 提供。
 */
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

/*
 * ISR 表。
 *
 * 数组下标就是 CPU 的异常向量号。
 *
 * 例如：
 *
 * isr_table[0]  -> isr0
 * isr_table[14] -> isr14
 * isr_table[31] -> isr31
 */
static isr_handler_t isr_table[32] = {
    isr0,
    isr1,
    isr2,
    isr3,
    isr4,
    isr5,
    isr6,
    isr7,
    isr8,
    isr9,
    isr10,
    isr11,
    isr12,
    isr13,
    isr14,
    isr15,
    isr16,
    isr17,
    isr18,
    isr19,
    isr20,
    isr21,
    isr22,
    isr23,
    isr24,
    isr25,
    isr26,
    isr27,
    isr28,
    isr29,
    isr30,
    isr31
};

/*
 * x86-64 一共有 256 个中断向量。
 *
 * 当前只使用前 32 个 CPU 异常。
 * 后面的硬件中断和软件中断以后再配置。
 */
static struct idt_entry idt[256];

static struct idt_pointer idt_ptr;

/*
 * 设置一个 IDT 门描述符。
 *
 * vector:
 *     中断 / 异常向量号。
 *
 * handler:
 *     处理函数地址。
 *
 * selector:
 *     GDT 中的代码段选择子。
 *     当前内核代码段为 0x08。
 *
 * attributes:
 *     当前使用 0x8E：
 *
 *     Present = 1
 *     DPL     = 0
 *     Type    = 64-bit Interrupt Gate
 */
static void idt_set_entry(
    int vector,
    void (*handler)(void),
    uint16_t selector,
    uint8_t attributes
)
{
    /*
     * 将函数指针转换成 64 位地址。
     */
    uint64_t handler_address =
        (uint64_t)handler;

    /*
     * 64 位处理函数地址在门描述符中分成三个字段，
     * selector 指向 GDT 的内核代码段，CPU 进入处理函数时使用它。
     */
    /*
     * IDT 不会连续保存完整的 64 位地址，
     * 而是拆成低 16 位、中间 16 位和高 32 位。
     */
    idt[vector].offset_low =
        handler_address & 0xFFFF;

    idt[vector].selector =
        selector;

    /*
     * 当前不使用 IST。
     *
     * 以后加入 TSS 后，可以给某些关键异常
     * 配置独立的异常栈。
     */
    idt[vector].ist =
        0;

    idt[vector].attributes =
        attributes;

    idt[vector].offset_middle =
        (handler_address >> 16) & 0xFFFF;

    idt[vector].offset_high =
        (handler_address >> 32) & 0xFFFFFFFF;

    /*
     * 保留字段必须为 0。
     */
    idt[vector].reserved =
        0;
}

/*
 * 将 IDT 加载到 CPU 的 IDTR。
 */
static void idt_load(void)
{
    /*
     * IDT 一共有 256 个 entry。
     *
     * limit 必须是整个表的字节数减 1。
     */
    idt_ptr.limit =
        sizeof(idt) - 1;

    /*
     * 保存 IDT 的内存地址。
     */
    idt_ptr.base =
        (uint64_t)&idt;

    /* lidt 只读取 IDTR 伪描述符，不会复制 IDT 内容。 */
    /*
     * lidt 将 IDT 的位置告诉 CPU。
     */
    __asm__ volatile(
        "lidt %0"
        :
        : "m"(idt_ptr)
    );
}

/*
 * 初始化 IDT。
 */
void idt_init(void)
{
    /*
     * CPU 保留的异常向量是 0~31。
     *
     * 每一个向量对应一个 ISR。
     *
     * 0x08：
     *     GDT 中的内核代码段。
     *
     * 0x8E：
     *     64 位 Interrupt Gate。
     */
    for (int i = 0; i < 32; i++)
    {
        idt_set_entry(
            i,
            isr_table[i],
            0x08,
            0x8E
        );
    }

    /* 完成加载后，CPU 才会按照这些门分派异常。 */
    /*
     * 把 IDT 地址加载到 CPU。
     */
    idt_load();
}
