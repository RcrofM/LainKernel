#include <kernel/interrupts.h>
#include <kernel/kernel.h>
#include "../drivers/vga.h"
#include "../lib/string.h"

struct idt_entry idt_entries[256];
struct idt_ptr idt_ptr_struct;

static isr_t interrupt_handlers[256];

static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void idt_set_gate(u8 num, u32 base, u16 selector, u8 flags) {
    idt_entries[num].base_low = base & 0xFFFF;
    idt_entries[num].base_high = (base >> 16) & 0xFFFF;
    idt_entries[num].selector = selector;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags = flags;
}

void idt_init(void) {
    idt_ptr_struct.limit = sizeof(struct idt_entry) * 256 - 1;
    idt_ptr_struct.base = (u32)&idt_entries;
    
    memset(&idt_entries, 0, sizeof(struct idt_entry) * 256);
    memset(&interrupt_handlers, 0, sizeof(isr_t) * 256);
    
    // Remap PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);
    
    // Install ISRs (exceptions)
    idt_set_gate(0, (u32)isr0, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(1, (u32)isr1, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(2, (u32)isr2, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(3, (u32)isr3, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(4, (u32)isr4, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(5, (u32)isr5, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(6, (u32)isr6, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(7, (u32)isr7, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(8, (u32)isr8, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(9, (u32)isr9, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(10, (u32)isr10, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(11, (u32)isr11, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(12, (u32)isr12, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(13, (u32)isr13, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(14, (u32)isr14, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(15, (u32)isr15, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(16, (u32)isr16, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(17, (u32)isr17, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(18, (u32)isr18, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(19, (u32)isr19, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(20, (u32)isr20, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(21, (u32)isr21, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(22, (u32)isr22, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(23, (u32)isr23, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(24, (u32)isr24, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(25, (u32)isr25, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(26, (u32)isr26, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(27, (u32)isr27, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(28, (u32)isr28, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(29, (u32)isr29, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(30, (u32)isr30, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(31, (u32)isr31, KERNEL_CODE_SEGMENT, 0x8E);
    
    // Install IRQs
    idt_set_gate(32, (u32)irq0, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(33, (u32)irq1, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(34, (u32)irq2, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(35, (u32)irq3, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(36, (u32)irq4, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(37, (u32)irq5, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(38, (u32)irq6, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(39, (u32)irq7, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(40, (u32)irq8, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(41, (u32)irq9, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(42, (u32)irq10, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(43, (u32)irq11, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(44, (u32)irq12, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(45, (u32)irq13, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(46, (u32)irq14, KERNEL_CODE_SEGMENT, 0x8E);
    idt_set_gate(47, (u32)irq15, KERNEL_CODE_SEGMENT, 0x8E);
    
    idt_flush((u32)&idt_ptr_struct);
}

void register_interrupt_handler(u8 n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

void isr_handler(struct registers regs) {
    if (interrupt_handlers[regs.int_no] != 0) {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(&regs);
    } else {
        vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
        vga_writestring("\n!!! EXCEPTION: ");
        vga_writestring(exception_messages[regs.int_no]);
        vga_writestring(" !!!\n");
        
        char buf[32];
        vga_writestring("Error Code: 0x");
        utoa(regs.err_code, buf, 16);
        vga_writestring(buf);
        vga_writestring("\nEIP: 0x");
        utoa(regs.eip, buf, 16);
        vga_writestring(buf);
        vga_writestring("\nCS: 0x");
        utoa(regs.cs, buf, 16);
        vga_writestring(buf);
        vga_writestring("\n");
        
        kernel_panic("Unhandled exception");
    }
}

void irq_handler(struct registers regs) {
    // Send EOI to PICs
    if (regs.int_no >= 40) {
        outb(0xA0, 0x20);  // Send EOI to slave
    }
    outb(0x20, 0x20);  // Send EOI to master
    
    if (interrupt_handlers[regs.int_no] != 0) {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(&regs);
    }
}
