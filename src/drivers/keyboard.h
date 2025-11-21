#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "kernel.h"

#define KEYBOARD_BUFFER_SIZE 256

// Keyboard functions
void keyboard_init(void);
char keyboard_getchar(void);
bool keyboard_has_input(void);
void keyboard_handler(struct registers* regs);

#endif // KEYBOARD_H
