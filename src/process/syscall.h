#ifndef SYSCALL_H
#define SYSCALL_H

#include "kernel.h"
#include "interrupts.h"

// System call numbers
#define SYS_WRITE 1
#define SYS_READ  2
#define SYS_EXIT  3

// System call functions
void syscall_init(void);
void syscall_handler(struct registers* regs);

#endif // SYSCALL_H
