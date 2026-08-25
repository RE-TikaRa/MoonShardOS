#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdint.h>

void console_init(void);

void console_clear(void);

void console_putchar(char character);

void console_write(const char *text);

void console_set_color(uint32_t color);

#endif