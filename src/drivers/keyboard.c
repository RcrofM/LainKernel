};

static const char scancode_to_ascii_shift[] = {
    0, 0, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
    0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,
    '*', 0, ' '
};

void keyboard_handler(struct registers* regs) {
    (void)regs;
    
    u8 scancode = inb(0x60);
    
    // Handle special keys
    if (scancode == 0x2A || scancode == 0x36) {  // Shift pressed
        shift_pressed = true;
        return;
    } else if (scancode == 0xAA || scancode == 0xB6) {  // Shift released
        shift_pressed = false;
        return;
    } else if (scancode == 0x1D) {  // Ctrl pressed
        ctrl_pressed = true;
        return;
    } else if (scancode == 0x9D) {  // Ctrl released
        ctrl_pressed = false;
        return;
    }
    
    // Only handle key press (not release)
    if (scancode & 0x80) {
        return;
    }
    
    // Convert scancode to ASCII
    char c = 0;
    if (scancode < sizeof(scancode_to_ascii)) {
        if (shift_pressed) {
            c = scancode_to_ascii_shift[scancode];
        } else {
            c = scancode_to_ascii[scancode];
        }
    }
    
    if (c != 0) {
        // Add to buffer
        u32 next_write = (buffer_write + 1) % KEYBOARD_BUFFER_SIZE;
        if (next_write != buffer_read) {
            keyboard_buffer[buffer_write] = c;
            buffer_write = next_write;
        }
    }
}

void keyboard_init(void) {
    register_interrupt_handler(33, keyboard_handler);
}

bool keyboard_has_input(void) {
    return buffer_read != buffer_write;
}

char keyboard_getchar(void) {
    while (!keyboard_has_input()) {
        __asm__ volatile("hlt");
    }
    
    char c = keyboard_buffer[buffer_read];
    buffer_read = (buffer_read + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}
