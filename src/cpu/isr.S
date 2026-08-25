.section .text

.extern isr_handler

/*
 * 导出 32 个 CPU 异常入口。
 */
.global isr0
.global isr1
.global isr2
.global isr3
.global isr4
.global isr5
.global isr6
.global isr7
.global isr8
.global isr9
.global isr10
.global isr11
.global isr12
.global isr13
.global isr14
.global isr15
.global isr16
.global isr17
.global isr18
.global isr19
.global isr20
.global isr21
.global isr22
.global isr23
.global isr24
.global isr25
.global isr26
.global isr27
.global isr28
.global isr29
.global isr30
.global isr31

/*
 * CPU 没有自动压入 Error Code 的异常。
 *
 * 统一补一个 0，使 C 层看到的结构保持一致。
 */
.macro ISR_NO_ERROR number

isr\number:
    pushq $0
    pushq $\number
    jmp isr_common

.endm

/*
 * CPU 已经自动压入 Error Code 的异常。
 */
.macro ISR_ERROR number

isr\number:
    pushq $\number
    jmp isr_common

.endm

ISR_NO_ERROR 0
ISR_NO_ERROR 1
ISR_NO_ERROR 2
ISR_NO_ERROR 3
ISR_NO_ERROR 4
ISR_NO_ERROR 5
ISR_NO_ERROR 6
ISR_NO_ERROR 7

ISR_ERROR 8

ISR_NO_ERROR 9

ISR_ERROR 10
ISR_ERROR 11
ISR_ERROR 12
ISR_ERROR 13
ISR_ERROR 14

ISR_NO_ERROR 15
ISR_NO_ERROR 16

ISR_ERROR 17

ISR_NO_ERROR 18
ISR_NO_ERROR 19
ISR_NO_ERROR 20

/*
 * #21 Control Protection Exception。
 */
ISR_ERROR 21

ISR_NO_ERROR 22
ISR_NO_ERROR 23
ISR_NO_ERROR 24
ISR_NO_ERROR 25
ISR_NO_ERROR 26
ISR_NO_ERROR 27
ISR_NO_ERROR 28

/*
 * #29 VMM Communication Exception。
 *
 * 在 AMD SEV-ES 环境中由硬件提供 Error Code。
 */
ISR_ERROR 29

ISR_ERROR 30

ISR_NO_ERROR 31

/*
 * 所有异常统一从这里进入。
 */
isr_common:

    /*
     * 暂停可屏蔽中断。
     */
    cli

    /*
     * 保存全部通用寄存器。
     *
     * 保存顺序与 C 层 trap_frame_t
     * 完全对应。
     */
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rsi
    pushq %rdi
    pushq %rbp
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    /*
     * 保存当前异常帧地址。
     *
     * RSP 此时指向 r15。
     */
    movq %rsp, %rdi

    /*
     * 使用 RBX 暂存原始 RSP。
     *
     * RBX 的原始值已经保存在栈中。
     * C 函数按照 System V AMD64 ABI
     * 必须保持 RBX，因此这里可以安全使用。
     */
    movq %rsp, %rbx

    /*
     * 调整给 C 函数的栈。
     *
     * 无论异常发生前 RSP 是什么对齐状态，
     * 这里都把调用前的 RSP 调整到 16 字节边界。
     */
    andq $-16, %rsp

    /*
     * 调用 C 层异常处理函数。
     *
     * RDI 仍然保存原始 trap frame 地址。
     */
    call isr_handler

    /*
     * 恢复异常现场原来的栈位置。
     */
    movq %rbx, %rsp

    /*
     * 恢复通用寄存器。
     *
     * 与上面的 push 顺序反过来。
     */
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rbp
    popq %rdi
    popq %rsi
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax

    /*
     * 删除：
     *
     * vector number
     * error code
     *
     * 总共 16 字节。
     */
    addq $16, %rsp

    /*
     * 从 CPU 异常返回。
     */
    iretq