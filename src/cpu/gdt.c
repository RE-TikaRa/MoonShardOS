#include "gdt.h"
#include <stdint.h>

struct gdt_entry
{
    /* 32 位段基址被拆成 low/middle/high 三段存储。 */
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

/* lgdt 使用的伪描述符：表大小减一和表首地址。 */
struct gdt_pointer
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

/*
 * 只有三个描述符：空描述符、内核代码段和内核数据段。
 * 因此它们对应的选择子分别是 0x00、0x08 和 0x10。
 */
static struct gdt_entry gdt[3];

static struct gdt_pointer gdt_ptr;

static void gdt_set_entry(
    int index,
    uint32_t base,
    uint32_t limit,
    uint8_t access,
    uint8_t granularity
)
{
    /*
     * 按 Intel 的描述符格式拆分基址和段界限。
     * granularity 的低四位来自 limit 的高 4 位，高四位保留调用者
     * 传入的粒度、默认操作数大小等标志。
     */
    gdt[index].base_low =
        base & 0xFFFF;

    gdt[index].base_middle =
        (base >> 16) & 0xFF;

    gdt[index].base_high =
        (base >> 24) & 0xFF;

    gdt[index].limit_low =
        limit & 0xFFFF;

    gdt[index].granularity =
        (limit >> 16) & 0x0F;

    gdt[index].granularity |=
        granularity & 0xF0;

    gdt[index].access =
        access;
}

static void gdt_load(void)
{
    /* GDTR limit 是字节数减一，而不是描述符数量。 */
    gdt_ptr.limit =
        (sizeof(struct gdt_entry) * 3) - 1;

    gdt_ptr.base =
        (uint64_t)&gdt;

    __asm__ volatile(
        "lgdt %0"
        :
        : "m"(gdt_ptr)
    );

    /*
     * 数据段寄存器可以直接改成数据段选择子 0x10。
     * CS 不能用 mov 写入，必须通过远返回重新加载代码段选择子。
     */
    __asm__ volatile(
        "mov $0x10, %%ax\n"
        "mov %%ax, %%ds\n"
        "mov %%ax, %%es\n"
        "mov %%ax, %%fs\n"
        "mov %%ax, %%gs\n"
        "mov %%ax, %%ss\n"
        :
        :
        : "%ax"
    );

    /*
     * 压入新的 CS 和下一条指令地址，再执行 lretq，
     * 以便 CPU 依据新的 GDT 重新装载 64 位内核代码段。
     */
    __asm__ volatile(
        "pushq $0x08\n"
        "lea 1f(%%rip), %%rax\n"
        "pushq %%rax\n"
        "lretq\n"
        "1:\n"
        :
        :
        : "%rax"
    );
}

void gdt_init(void)
{
    /* index 0 必须保持为空，供硬件识别无效选择子。 */
    gdt_set_entry(
        0,
        0,
        0,
        0x00,
        0x00
    );

    /*
     * 0x9A = Present | Ring 0 | Code | Readable。
     * 0xA0 设置长模式代码段需要的粒度/长模式相关标志。
     */
    gdt_set_entry(
        1,
        0,
        0x000FFFFF,
        0x9A,
        0xA0
    );

    /*
     * 0x92 = Present | Ring 0 | Data | Writable。
     * 数据段同样覆盖整个线性地址空间；在 64 位模式下基址和界限
     * 对普通数据访问基本被忽略，但描述符仍必须有效。
     */
    gdt_set_entry(
        2,
        0,
        0x000FFFFF,
        0x92,
        0xA0
    );

    gdt_load();
}
