#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/* 建立 CPU 异常使用的前 32 个 IDT 门，并加载 IDTR。 */
void idt_init();

#endif
