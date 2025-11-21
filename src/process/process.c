#include "process.h"`n#include <kernel/kernel.h>`n#include <kernel/memory.h>`n#include "../lib/string.h"
#include "string.h"

static process_t processes[MAX_PROCESSES];
static process_t* current_process = 0;
static process_t* ready_queue = 0;
static u32 next_pid = 1;

void process_init(void) {
    memset(processes, 0, sizeof(processes));
    current_process = 0;
    ready_queue = 0;
    next_pid = 1;
}

process_t* process_create(void (*entry_point)(void), u8 privilege_level) {
    // Find free process slot
    process_t* proc = 0;
    for (u32 i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_STATE_TERMINATED || processes[i].pid == 0) {
            proc = &processes[i];
            break;
        }
    }
    
    if (!proc) {
        return 0;  // No free slots
    }
    
    // Initialize process
    proc->pid = next_pid++;
    proc->state = PROCESS_STATE_READY;
    proc->privilege_level = privilege_level;
    
    // Clone kernel page directory for now
    // In a real implementation, create separate user space
    proc->page_directory = paging_get_kernel_directory();
    
    // Allocate kernel stack
    proc->kernel_stack = (u32)kmalloc(4096) + 4096;
    
    // Set up initial stack frame
    proc->esp = proc->kernel_stack;
    proc->ebp = proc->kernel_stack;
    proc->eip = (u32)entry_point;
    
    // Add to ready queue
    proc->next = ready_queue;
    ready_queue = proc;
    
    return proc;
}

void process_terminate(u32 pid) {
    for (u32 i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = PROCESS_STATE_TERMINATED;
            
            // Remove from ready queue
            if (ready_queue == &processes[i]) {
                ready_queue = processes[i].next;
            } else {
                process_t* prev = ready_queue;
                while (prev && prev->next != &processes[i]) {
                    prev = prev->next;
                }
                if (prev) {
                    prev->next = processes[i].next;
                }
            }
            
            // Free kernel stack
            if (processes[i].kernel_stack) {
                kfree((void*)(processes[i].kernel_stack - 4096));
            }
            
            break;
        }
    }
}

void process_yield(void) {
    // Simple round-robin scheduling
    if (!current_process || !ready_queue) {
        return;
    }
    
    current_process->state = PROCESS_STATE_READY;
    process_schedule();
}

void process_schedule(void) {
    if (!ready_queue) {
        return;
    }
    
    // Get next ready process
    process_t* next = ready_queue;
    ready_queue = next->next;
    
    // Add current process back to queue if still ready
    if (current_process && current_process->state == PROCESS_STATE_READY) {
        current_process->next = ready_queue;
        ready_queue = current_process;
    }
    
    // Switch to next process
    current_process = next;
    current_process->state = PROCESS_STATE_RUNNING;
    
    // Switch page directory
    paging_switch_directory(current_process->page_directory);
    
    // Update TSS for privilege level switching
    tss_set_kernel_stack(KERNEL_DATA_SEGMENT, current_process->kernel_stack);
}

process_t* process_get_current(void) {
    return current_process;
}

