#include "../console/console.h"

#include <stdint.h>

/*
 * CPU 异常发生后，isr.S 会按照固定顺序
 * 保存通用寄存器、异常向量和错误码。
 *
 * 下面的结构必须与 isr.S 的栈布局保持一致。
 */
struct trap_frame
{
    /* isr_common 保存的通用寄存器 */
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;

    /* ISR 保存 */
    uint64_t vector;
    uint64_t error_code;

    /* CPU 自动保存 */
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
};

/*
 * 读取 CR2。
 *
 * CR2 保存触发 Page Fault (#PF, vector 14)
 * 的线性虚拟地址。
 */
static uint64_t read_cr2(void)
{
    uint64_t value;

    __asm__ volatile(
        "mov %%cr2, %0"
        : "=r"(value)
    );

    return value;
}

/*
 * CPU 异常处理入口。
 */
void isr_handler(struct trap_frame *frame)
{
    console_write(
        "\n\nCPU EXCEPTION\n"
    );

    console_write(
        "Kernel stopped.\n"
    );

    /*
     * Page Fault 特殊处理。
     *
     * CR2 中保存触发缺页异常的虚拟地址。
     */
    if (frame->vector == 14)
    {
        uint64_t fault_address =
            read_cr2();

        (void)fault_address;
    }

    (void)frame;

    for (;;)
    {
        __asm__ volatile("hlt");
    }
}