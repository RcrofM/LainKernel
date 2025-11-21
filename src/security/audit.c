#include "audit.h"`n#include <kernel/kernel.h>`n#include "../drivers/vga.h"`n#include "../lib/string.h"
#include "vga.h"
#include "string.h"

#define AUDIT_LOG_SIZE 1024

static audit_log_entry_t audit_log[AUDIT_LOG_SIZE];
static u32 audit_log_index = 0;
static u32 audit_tick_count = 0;

static const char* audit_event_names[] = {
    "SYSCALL",
    "PRIVILEGE_VIOLATION",
    "MEMORY_VIOLATION",
    "INVALID_POINTER",
    "STACK_SMASH",
    "PROCESS_CREATE",
    "PROCESS_TERMINATE",
    "RATE_LIMIT_EXCEEDED",
    "KERNEL_PANIC"
};

void audit_init(void) {
    memset(audit_log, 0, sizeof(audit_log));
    audit_log_index = 0;
    audit_tick_count = 0;
}

void audit_log_event(audit_event_type_t type, u32 data0, u32 data1, u32 data2, u32 data3) {
    audit_log_entry_t* entry = &audit_log[audit_log_index];
    
    entry->timestamp = audit_tick_count++;
    entry->type = type;
    entry->pid = 0;  // Will be set by caller if needed
    entry->data[0] = data0;
    entry->data[1] = data1;
    entry->data[2] = data2;
    entry->data[3] = data3;
    
    audit_log_index = (audit_log_index + 1) % AUDIT_LOG_SIZE;
}

void audit_print_log(void) {
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_writestring("\n=== Audit Log ===\n");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    
    u32 start = audit_log_index;
    u32 count = 0;
    
    // Show last 20 entries
    for (u32 i = 0; i < 20 && i < AUDIT_LOG_SIZE; i++) {
        u32 idx = (start + AUDIT_LOG_SIZE - 20 + i) % AUDIT_LOG_SIZE;
        audit_log_entry_t* entry = &audit_log[idx];
        
        if (entry->timestamp == 0) continue;
        
        char buf[32];
        vga_writestring("[");
        utoa(entry->timestamp, buf, 10);
        vga_writestring(buf);
        vga_writestring("] ");
        
        if (entry->type < sizeof(audit_event_names) / sizeof(char*)) {
            vga_writestring(audit_event_names[entry->type]);
        } else {
            vga_writestring("UNKNOWN");
        }
        
        vga_writestring(" - Data: 0x");
        utoa(entry->data[0], buf, 16);
        vga_writestring(buf);
        vga_writestring("\n");
        
        count++;
    }
    
    if (count == 0) {
        vga_writestring("No audit events logged.\n");
    }
    vga_writestring("\n");
}

u32 audit_get_event_count(audit_event_type_t type) {
    u32 count = 0;
    for (u32 i = 0; i < AUDIT_LOG_SIZE; i++) {
        if (audit_log[i].type == type && audit_log[i].timestamp > 0) {
            count++;
        }
    }
    return count;
}

