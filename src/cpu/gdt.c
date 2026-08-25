#include "gdt.h"
#include <stdint.h>

struct gdt_entry
{
    uint16_t limit_low;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_pointer
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

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
    gdt_ptr.limit =
        (sizeof(struct gdt_entry) * 3) - 1;

    gdt_ptr.base =
        (uint64_t)&gdt;

    __asm__ volatile(
        "lgdt %0"
        :
        : "m"(gdt_ptr)
    );

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
    gdt_set_entry(
        0,
        0,
        0,
        0x00,
        0x00
    );
    gdt_set_entry(
        1,
        0,
        0x000FFFFF,
        0x9A,
        0xA0
    );
    gdt_set_entry(
        2,
        0,
        0x000FFFFF,
        0x92,
        0xA0
    );

    gdt_load();
}