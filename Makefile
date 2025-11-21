AS = nasm
CC = gcc
LD = ld

SRC_DIR = src
BUILD_DIR = build
ISO_DIR = iso
INCLUDE_DIR = include

ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -I$(INCLUDE_DIR) \
         -fno-exceptions -fno-stack-protector -nostdlib -nostdinc -fno-builtin
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

ASM_SOURCES = $(SRC_DIR)/arch/x86/boot.asm \
              $(SRC_DIR)/interrupts/isr.asm

C_SOURCES = $(SRC_DIR)/kernel/kernel.c \
            $(SRC_DIR)/mm/memory.c \
            $(SRC_DIR)/interrupts/interrupts.c \
            $(SRC_DIR)/drivers/vga.c \
            $(SRC_DIR)/drivers/keyboard.c \
            $(SRC_DIR)/security/security.c \
            $(SRC_DIR)/security/random.c \
            $(SRC_DIR)/security/audit.c \
            $(SRC_DIR)/process/process.c \
            $(SRC_DIR)/process/syscall.c \
            $(SRC_DIR)/lib/string.c

ASM_OBJECTS = $(BUILD_DIR)/arch/x86/boot.o \
              $(BUILD_DIR)/interrupts/isr.o

C_OBJECTS = $(BUILD_DIR)/kernel/kernel.o \
            $(BUILD_DIR)/mm/memory.o \
            $(BUILD_DIR)/interrupts/interrupts.o \
            $(BUILD_DIR)/drivers/vga.o \
            $(BUILD_DIR)/drivers/keyboard.o \
            $(BUILD_DIR)/security/security.o \
            $(BUILD_DIR)/security/random.o \
            $(BUILD_DIR)/security/audit.o \
            $(BUILD_DIR)/process/process.o \
            $(BUILD_DIR)/process/syscall.o \
            $(BUILD_DIR)/lib/string.o

OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

KERNEL = $(BUILD_DIR)/kernel.bin
ISO = lainkernel.iso

.PHONY: all clean run dirs

all: dirs $(ISO)

dirs:
	@if not exist "$(BUILD_DIR)\kernel" mkdir "$(BUILD_DIR)\kernel"
	@if not exist "$(BUILD_DIR)\mm" mkdir "$(BUILD_DIR)\mm"
	@if not exist "$(BUILD_DIR)\arch\x86" mkdir "$(BUILD_DIR)\arch\x86"
	@if not exist "$(BUILD_DIR)\interrupts" mkdir "$(BUILD_DIR)\interrupts"
	@if not exist "$(BUILD_DIR)\drivers" mkdir "$(BUILD_DIR)\drivers"
	@if not exist "$(BUILD_DIR)\security" mkdir "$(BUILD_DIR)\security"
	@if not exist "$(BUILD_DIR)\process" mkdir "$(BUILD_DIR)\process"
	@if not exist "$(BUILD_DIR)\lib" mkdir "$(BUILD_DIR)\lib"
	@if not exist "$(ISO_DIR)\boot\grub" mkdir "$(ISO_DIR)\boot\grub"

$(ISO): $(KERNEL)
	copy "$(KERNEL)" "$(ISO_DIR)\boot\kernel.bin"
	grub-mkrescue -o $(ISO) $(ISO_DIR)

$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/arch/x86/boot.o: $(SRC_DIR)/arch/x86/boot.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/interrupts/isr.o: $(SRC_DIR)/interrupts/isr.asm
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/kernel/kernel.o: $(SRC_DIR)/kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/mm/memory.o: $(SRC_DIR)/mm/memory.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/interrupts/interrupts.o: $(SRC_DIR)/interrupts/interrupts.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/drivers/vga.o: $(SRC_DIR)/drivers/vga.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/drivers/keyboard.o: $(SRC_DIR)/drivers/keyboard.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/security/security.o: $(SRC_DIR)/security/security.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/security/random.o: $(SRC_DIR)/security/random.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/security/audit.o: $(SRC_DIR)/security/audit.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/process/process.o: $(SRC_DIR)/process/process.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/process/syscall.o: $(SRC_DIR)/process/syscall.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lib/string.o: $(SRC_DIR)/lib/string.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	if exist "$(BUILD_DIR)" rmdir /s /q "$(BUILD_DIR)"
	if exist "$(ISO)" del "$(ISO)"

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)
