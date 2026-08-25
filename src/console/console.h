#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

/*
 * 建立在 framebuffer 和 8x8 字体之上的内核文字输出接口。
 * Console 自己维护光标位置和当前颜色，调用者只需要提供字符流。
 */

/* 将光标归零，并设置默认文字颜色。 */
void console_init(void);

/* 清屏并把光标移回左上角。 */
void console_clear(void);

/* 输出一个字符，处理换行、回车、自动换行和屏幕底部回卷。 */
void console_putchar(char character);

/* 连续输出以空字符结尾的字符串。 */
void console_write(const char *text);

/* 设置后续字符使用的 framebuffer 原生颜色。 */
void console_set_color(uint32_t color);

#endif
