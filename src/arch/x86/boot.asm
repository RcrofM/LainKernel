; Multiboot2 header constants
MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
FLAGS    equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

; Declare multiboot header
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Reserve stack space
section .bss
align 16
stack_bottom:
    resb 16384  ; 16 KiB
stack_top:

; Kernel entry point
section .text
global _start
extern kernel_main

_start:
    ; Set up stack
    mov esp, stack_top
    
    ; Push multiboot info
    push ebx
    push eax
    
    ; Call kernel main
    call kernel_main
    
    ; Hang if kernel returns
    cli
.hang:
    hlt
    jmp .hang

; GDT flush function
global gdt_flush
extern gdt_ptr_struct

gdt_flush:
    mov eax, [esp+4]
    lgdt [eax]
    
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 0x08:.flush
.flush:
    ret

; TSS flush function
global tss_flush

tss_flush:
    mov ax, 0x2B  ; TSS segment selector (0x28 | 3 for Ring 3)
    ltr ax
    ret

; IDT flush function
global idt_flush

idt_flush:
    mov eax, [esp+4]
    lidt [eax]
    ret
