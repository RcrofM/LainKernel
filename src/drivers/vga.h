#ifndef VGA_H
#define VGA_H

#include "kernel.h"

// VGA colors
enum vga_color {
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
};

// VGA dimensions
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

// VGA functions
void vga_init(void);
void vga_clear(void);
void vga_putchar(char c);
void vga_write(const char* data, size_t size);
void vga_writestring(const char* data);
void vga_set_color(u8 fg, u8 bg);
void vga_enable_cursor(u8 cursor_start, u8 cursor_end);
void vga_disable_cursor(void);
void vga_update_cursor(int x, int y);

#endif // VGA_H
