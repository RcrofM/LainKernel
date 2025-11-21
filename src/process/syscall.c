#include "syscall.h"`n#include <kernel/kernel.h>`n#include <kernel/memory.h>`n#include "../security/security.h"`n#include "../drivers/vga.h"`n#include "process.h"`n#include "../lib/string.h"`n#include "../security/audit.h"
#include "memory.h"
#include "security.h"
#include "vga.h"
#include "process.h"
#include "audit.h"

static void sys_write(const char* str, u32 len) {
    if (!memory_validate_user_ptr(str, len)) {
        audit_log_event(AUDIT_INVALID_POINTER, (u32)str, len, 0, 0);
        return;
    }
    

    char* safe_str = (char*)kmalloc(len + 1);
    if (!safe_str) {
        return;
    }
    
    memcpy(safe_str, str, len);
    safe_str[len] = '\0';
    security_sanitize_string(safe_str, len + 1);
    
    vga_writestring(safe_str);
    kfree(safe_str);
}

static void sys_read(char* buf, u32 len) {
    if (!memory_validate_user_ptr(buf, len)) {
        audit_log_event(AUDIT_INVALID_POINTER, (u32)buf, len, 0, 0);
        return;
    }
    

    memset(buf, 0, len);
}

static void sys_exit(u32 code) {
    (void)code;
    process_t* current = process_get_current();
    if (current) {
        process_terminate(current->pid);
    }
}

void syscall_handler(struct registers* regs) {
    if (!security_validate_privilege(RING_3)) {
        audit_log_event(AUDIT_PRIVILEGE_VIOLATION, regs->eax, 0, 0, 0);
        return;
    }
    

    audit_log_event(AUDIT_SYSCALL, regs->eax, regs->ebx, regs->ecx, regs->edx);
    

    u32 syscall_num = regs->eax;
    
    switch (syscall_num) {
        case SYS_WRITE:
            sys_write((const char*)regs->ebx, regs->ecx);
            break;
        case SYS_READ:
            sys_read((char*)regs->ebx, regs->ecx);
            break;
        case SYS_EXIT:
            sys_exit(regs->ebx);
            break;
        default:
            break;
    }
}

void syscall_init(void) {

    idt_set_gate(0x80, (u32)syscall_handler, KERNEL_CODE_SEGMENT, 0xEE);
}

