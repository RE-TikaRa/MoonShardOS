#include "../console/console.h"

#include <stdint.h>

/*
 * isr.S 将硬件自动压入的返回帧、统一补齐的 vector/error_code，
 * 以及保存的通用寄存器拼成一个连续栈帧，再把栈首地址传给这里。
 * 该布局是汇编和 C 之间的 ABI，字段顺序改变会使异常信息错位。
 */
/*
 * CPU 异常发生后，isr.S 会按照固定顺序
 * 保存通用寄存器、异常向量和错误码。
 *
 * 下面的结构必须与 isr.S 的栈布局保持一致。
 */
struct trap_frame
{
    /* isr_common 按 push 顺序保存，栈顶到栈底为 r15 到 rax。 */
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

    /* ISR 入口统一保存的异常向量和错误码。 */
    uint64_t vector;
    uint64_t error_code;

    /* CPU 在进入门处理函数时自动保存的返回现场。 */
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

    /* CR2 保存 Page Fault 触发时的线性地址。 */
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
    /*
     * 所有 CPU 异常共用这个终止式处理器；输出依赖已经初始化的 Console。
     */
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
        /* Page Fault 的地址由 CPU 写入 CR2，而不是放在通用错误码中。 */
        uint64_t fault_address =
            read_cr2();

        /* 当前输出界面尚未显示地址，保留读取动作供后续诊断使用。 */
        (void)fault_address;
    }

    /* 异常处理目前不恢复现场，内核停在这里等待调试。 */
    for (;;)
    {
        __asm__ volatile("hlt");
    }
}
