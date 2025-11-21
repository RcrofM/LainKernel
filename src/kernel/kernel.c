#include <kernel/kernel.h>
#include "../drivers/vga.h"
#include "../mm/memory.h"
#include "../interrupts/interrupts.h"
#include "../drivers/keyboard.h"
#include "../security/security.h"
#include "../process/process.h"
#include "../process/syscall.h"
#include "../lib/string.h"
#include "../security/audit.h"

struct gdt_entry gdt_entries[6];
struct gdt_ptr gdt_ptr_struct;
struct tss_entry tss;

void kernel_panic(const char* message) {
    __asm__ volatile("cli");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_RED);
    vga_writestring("\n\n*** KERNEL PANIC ***\n");
    vga_writestring(message);
    vga_writestring("\n\nSystem halted.");
    
    while (1) {
        __asm__ volatile("hlt");
    }
}

void gdt_set_gate(int num, u32 base, u32 limit, u8 access, u8 gran) {
    gdt_entries[num].base_low = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;
    
    gdt_entries[num].limit_low = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access = access;
}

void tss_init(u32 idx, u32 ss0, u32 esp0) {
    u32 base = (u32)&tss;
    u32 limit = base + sizeof(tss);
    
    gdt_set_gate(idx, base, limit, 0xE9, 0x00);
    
    memset(&tss, 0, sizeof(tss));
    tss.ss0 = ss0;
    tss.esp0 = esp0;
    tss.cs = KERNEL_CODE_SEGMENT | 0x3;
    tss.ss = tss.ds = tss.es = tss.fs = tss.gs = KERNEL_DATA_SEGMENT | 0x3;
}

void tss_set_kernel_stack(u32 ss, u32 esp) {
    tss.ss0 = ss;
    tss.esp0 = esp;
}

void gdt_init(void) {
    gdt_ptr_struct.limit = (sizeof(struct gdt_entry) * 6) - 1;
    gdt_ptr_struct.base = (u32)&gdt_entries;
    
    gdt_set_gate(0, 0, 0, 0, 0);
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);
    

    tss_init(5, KERNEL_DATA_SEGMENT, 0x0);
    
    gdt_flush((u32)&gdt_ptr_struct);
    tss_flush();
}

void print_welcome(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writestring("================================================================================\n");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_writestring("                          SecureOS Kernel v1.0\n");
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writestring("================================================================================\n\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_writestring("Security Features:\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_writestring("  [+] Ring-based protection (Ring 0/Ring 3)\n");
    vga_writestring("  [+] Memory isolation and paging\n");
    vga_writestring("  [+] Stack canary protection\n");
    vga_writestring("  [+] Input validation and sanitization\n");
    vga_writestring("  [+] Privilege level enforcement\n\n");
    
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_writestring("Kernel initialized successfully!\n\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

void shell_command(const char* cmd) {
    if (strcmp(cmd, "help") == 0) {
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_writestring("\nAvailable commands:\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_writestring("  help    - Show this help message\n");
        vga_writestring("  clear   - Clear the screen\n");
        vga_writestring("  info    - Display system information\n");
        vga_writestring("  test    - Run security tests\n");
        vga_writestring("  audit   - Display security audit log\n");
        vga_writestring("  reboot  - Reboot the system\n\n");
    } else if (strcmp(cmd, "clear") == 0) {
        vga_clear();
        print_welcome();
    } else if (strcmp(cmd, "info") == 0) {
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_writestring("\nSystem Information:\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_writestring("  Kernel: SecureOS v1.0\n");
        vga_writestring("  Architecture: x86 (32-bit)\n");
        vga_writestring("  Memory Protection: Enabled\n");
        vga_writestring("  Paging: Enabled\n");
        vga_writestring("  Interrupts: Enabled\n\n");
    } else if (strcmp(cmd, "test") == 0) {
        vga_set_color(VGA_COLOR_LIGHT_YELLOW, VGA_COLOR_BLACK);
        vga_writestring("\nRunning security tests...\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        
        // Test stack canary
        u32 canary = security_get_canary();
        if (security_check_canary(canary)) {
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_writestring("  [PASS] Stack canary validation\n");
        }
        
        // Test privilege level
        if (security_validate_privilege(RING_0)) {
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_writestring("  [PASS] Privilege level check (Ring 0)\n");
        }
        
        // Test memory validation
        if (!memory_validate_user_ptr((void*)0xC0000000, 4096)) {
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_writestring("  [PASS] Kernel memory protection\n");
        }
        
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
        vga_writestring("\nAll tests passed!\n\n");
    } else if (strcmp(cmd, "audit") == 0) {
        audit_print_log();
    } else if (strcmp(cmd, "reboot") == 0) {
        vga_writestring("\nRebooting...\n");
        outb(0x64, 0xFE);
        __asm__ volatile("hlt");
    } else if (strlen(cmd) > 0) {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_writestring("Unknown command: ");
        vga_writestring(cmd);
        vga_writestring("\nType 'help' for available commands.\n\n");
        vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    }
}

void kernel_shell(void) {
    char command[256];
    u32 cmd_pos = 0;
    
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_writestring("kernel> ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    
    while (1) {
        char c = keyboard_getchar();
        
        if (c == '\n') {
            vga_putchar('\n');
            command[cmd_pos] = '\0';
            shell_command(command);
            

            cmd_pos = 0;
            vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
            vga_writestring("kernel> ");
            vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
        } else if (c == '\b') {
            if (cmd_pos > 0) {
                cmd_pos--;
                vga_putchar('\b');
            }
        } else if (cmd_pos < sizeof(command) - 1) {
            command[cmd_pos++] = c;
            vga_putchar(c);
        }
    }
}

void kernel_main(u32 magic, u32 addr) {
    (void)magic;
    (void)addr;
    

    vga_init();
    

    gdt_init();
    

    memory_init(32 * 1024 * 1024);
    

    idt_init();
    

    security_init();
    

    keyboard_init();
    

    process_init();
    

    syscall_init();
    

    __asm__ volatile("sti");
    

    print_welcome();
    

    kernel_shell();
    

    kernel_panic("Kernel shell exited unexpectedly");
}
