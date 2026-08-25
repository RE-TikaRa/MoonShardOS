#ifndef GDT_H
#define GDT_H

/* 建立内核使用的空描述符、代码段和数据段，并加载 GDTR。 */
void gdt_init(void);

#endif
