#ifndef AUDIT_H
#define AUDIT_H

#include "kernel.h"

// Audit event types
typedef enum {
    AUDIT_SYSCALL,
    AUDIT_PRIVILEGE_VIOLATION,
    AUDIT_MEMORY_VIOLATION,
    AUDIT_INVALID_POINTER,
    AUDIT_STACK_SMASH,
    AUDIT_PROCESS_CREATE,
    AUDIT_PROCESS_TERMINATE,
    AUDIT_RATE_LIMIT_EXCEEDED,
    AUDIT_KERNEL_PANIC
} audit_event_type_t;

// Audit log entry
typedef struct {
    u32 timestamp;
    audit_event_type_t type;
    u32 pid;
    u32 data[4];
} audit_log_entry_t;

// Audit functions
void audit_init(void);
void audit_log_event(audit_event_type_t type, u32 data0, u32 data1, u32 data2, u32 data3);
void audit_print_log(void);
u32 audit_get_event_count(audit_event_type_t type);

#endif // AUDIT_H
