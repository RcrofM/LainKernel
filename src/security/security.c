#include "security.h"`n#include "../lib/string.h"`n#include <kernel/memory.h>`n#include "random.h"`n#include "audit.h"
#include "memory.h"
#include "string.h"
#include "random.h"
#include "audit.h"

static u32 stack_canary = STACK_CANARY_VALUE;

void security_init(void) {
    random_init();
    stack_canary = random_get();
    audit_init();
}

u32 security_get_canary(void) {
    return stack_canary;
}

bool security_check_canary(u32 canary) {
    if (canary != stack_canary) {
        audit_log_event(AUDIT_STACK_SMASH, canary, stack_canary, 0, 0);
        kernel_panic("Stack smashing detected!");
        return false;
    }
    return true;
}

bool security_validate_privilege(u8 required_ring) {
    u16 cs;
    __asm__ volatile("mov %%cs, %0" : "=r"(cs));
    u8 current_ring = cs & 0x3;
    
    return current_ring <= required_ring;
}

void security_sanitize_string(char* str, u32 max_len) {
    if (!str) return;
    
    u32 len = 0;
    while (len < max_len && str[len] != '\0') {
        if (!security_is_printable(str[len])) {
            str[len] = '?';
        }
        len++;
    }
    
    if (len >= max_len) {
        str[max_len - 1] = '\0';
    }
}

bool security_is_printable(char c) {
    return (c >= 32 && c <= 126) || c == '\n' || c == '\r' || c == '\t';
}

bool security_validate_buffer(const void* buf, u32 size) {
    if (!buf || size == 0) {
        return false;
    }
    
    if ((u32)buf + size < (u32)buf) {
        return false;
    }
    
    return true;
}

