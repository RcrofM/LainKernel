#ifndef PROCESS_H
#define PROCESS_H

#include "kernel.h"
#include "memory.h"

#define MAX_PROCESSES 32

typedef enum {
    PROCESS_STATE_READY,
    PROCESS_STATE_RUNNING,
    PROCESS_STATE_BLOCKED,
    PROCESS_STATE_TERMINATED
} process_state_t;

typedef struct process {
    u32 pid;
    u32 esp;
    u32 ebp;
    u32 eip;
    page_directory_t* page_directory;
    process_state_t state;
    u8 privilege_level;
    u32 kernel_stack;
    struct process* next;
} process_t;

// Process management functions
void process_init(void);
process_t* process_create(void (*entry_point)(void), u8 privilege_level);
void process_terminate(u32 pid);
void process_yield(void);
void process_schedule(void);
process_t* process_get_current(void);

#endif // PROCESS_H
