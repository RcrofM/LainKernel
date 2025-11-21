#include "vga.h"
#include <kernel/kernel.h>
#include "../lib/string.h"

static const size_t VGA_BUFFER = 0xB8000;
static size_t terminal_row;
static size_t terminal_column;
static u8 terminal_color;
static u16* terminal_buffer;

static inline u8 vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}

static inline u16 vga_entry(unsigned char uc, u8 color) {
    return (u16) uc | (u16) color << 8;
}

void vga_init(void) {
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    terminal_buffer = (u16*) VGA_BUFFER;
    vga_clear();
    vga_enable_cursor(0, 15);
}

void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
    vga_update_cursor(0, 0);
}

void vga_set_color(u8 fg, u8 bg) {
    terminal_color = vga_entry_color(fg, bg);
}

static void vga_scroll(void) {
    // Move all lines up by one
    for (size_t y = 0; y < VGA_HEIGHT - 1; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = terminal_buffer[(y + 1) * VGA_WIDTH + x];
        }
    }
    
    // Clear the last line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        terminal_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = vga_entry(' ', terminal_color);
    }
    
    terminal_row = VGA_HEIGHT - 1;
}

void vga_putchar(char c) {
    if (c == '\n') {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT) {
            vga_scroll();
        }
    } else if (c == '\r') {
        terminal_column = 0;
    } else if (c == '\t') {
        terminal_column = (terminal_column + 8) & ~(8 - 1);
        if (terminal_column >= VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
    } else if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            const size_t index = terminal_row * VGA_WIDTH + terminal_column;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    } else {
        const size_t index = terminal_row * VGA_WIDTH + terminal_column;
        terminal_buffer[index] = vga_entry((unsigned char) c, terminal_color);
        if (++terminal_column == VGA_WIDTH) {
            terminal_column = 0;
            if (++terminal_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
    }
    vga_update_cursor(terminal_column, terminal_row);
}

void vga_write(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++)
        vga_putchar(data[i]);
}

void vga_writestring(const char* data) {
    vga_write(data, strlen(data));
}

void vga_enable_cursor(u8 cursor_start, u8 cursor_end) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, (inb(0x3D5) & 0xC0) | cursor_start);
    
    outb(0x3D4, 0x0B);
    outb(0x3D5, (inb(0x3D5) & 0xE0) | cursor_end);
}

void vga_disable_cursor(void) {
    outb(0x3D4, 0x0A);
    outb(0x3D5, 0x20);
}

void vga_update_cursor(int x, int y) {
    u16 pos = y * VGA_WIDTH + x;
    
    outb(0x3D4, 0x0F);
    outb(0x3D5, (u8) (pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (u8) ((pos >> 8) & 0xFF));
}
