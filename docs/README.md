# LainKernel

A basic x86 kernel written in C with comprehensive security features, designed for educational purposes and demonstrating fundamental OS concepts.

## Features

### Security Features

- **Ring-based Protection**: Implements x86 privilege levels (Ring 0 for kernel, Ring 3 for user mode)
- **Memory Isolation**: Full paging support with separate kernel and user address spaces
- **Stack Canaries**: Buffer overflow detection using cryptographically random stack canaries
- **CSPRNG**: Cryptographically Secure Pseudo-Random Number Generator (Xorshift128+)
- **Input Validation**: Comprehensive validation and sanitization of all user inputs
- **Memory Protection**: Validates all user-space pointers before dereferencing
- **Privilege Enforcement**: System calls validate caller privilege level
- **Audit Logging**: Security event tracking and logging system
- **Rate Limiting**: Protection against DoS via syscall rate limiting (ready for implementation)

### Core Components

- **Bootloader**: Multiboot2-compliant bootloader for GRUB
- **Memory Management**:
  - Physical memory manager with bitmap allocator
  - Virtual memory with paging (4KB pages)
  - Kernel heap allocator with corruption detection
  - Memory validation for security
- **Interrupt Handling**:
  - IDT (Interrupt Descriptor Table) setup
  - ISR (Interrupt Service Routine) handlers for CPU exceptions
  - IRQ handlers for hardware interrupts
  - PIC (Programmable Interrupt Controller) configuration
- **Process Management**:
  - Process Control Blocks (PCB)
  - Round-robin scheduler
  - Context switching support
  - Privilege level management
- **System Calls**:
  - System call interface (INT 0x80)
  - Input validation and sanitization
  - Safe kernel-user space data transfer
- **Drivers**:
  - VGA text mode driver with color support
  - PS/2 keyboard driver with scancode translation

## Prerequisites

To build and run this kernel, you need:

### Linux/WSL

```bash
# Install cross-compiler tools
sudo apt-get update
sudo apt-get install build-essential nasm grub-pc-bin xorriso qemu-system-x86

# For cross-compilation (optional but recommended)
sudo apt-get install gcc-multilib
```

### Windows (Native)

You'll need:
- MinGW-w64 or similar GCC toolchain with 32-bit support
- NASM assembler
- GRUB tools (can use WSL for this step)
- QEMU for testing

## Building

### Quick Build

```bash
make
```

This will:
1. Compile all assembly files (.asm → .o)
2. Compile all C files (.c → .o)
3. Link everything into `kernel.bin`
4. Create a bootable ISO image `kernel.iso`

### Clean Build

```bash
make clean
make
```

## Running

### Using QEMU (Recommended)

```bash
make run
```

Or manually:

```bash
qemu-system-i386 -cdrom kernel.iso
```

### Using VirtualBox

1. Create a new VM (Type: Other, Version: Other/Unknown)
2. Allocate at least 32MB RAM
3. Attach `kernel.iso` as a CD-ROM
4. Boot the VM

### Using Real Hardware

⚠️ **Warning**: Only for advanced users!

1. Burn `kernel.iso` to a USB drive or CD
2. Boot from the media
3. The kernel should load via GRUB

## Usage

Once booted, you'll see the LainKernel welcome screen. The kernel provides an interactive shell with the following commands:

- `help` - Display available commands
- `clear` - Clear the screen
- `info` - Show system information
- `test` - Run security tests
- `audit` - Display security audit log
- `reboot` - Reboot the system

### Example Session

```
kernel> help
Available commands:
  help    - Show this help message
  clear   - Clear the screen
  info    - Display system information
  test    - Run security tests
  reboot  - Reboot the system

kernel> test
Running security tests...
  [PASS] Stack canary validation
  [PASS] Privilege level check (Ring 0)
  [PASS] Kernel memory protection

All tests passed!

kernel> info
System Information:
  Kernel: LainKernel v1.0
  Architecture: x86 (32-bit)
  Memory Protection: Enabled
  Paging: Enabled
  Interrupts: Enabled
```

## Architecture

### Memory Layout

```
0x00000000 - 0x000FFFFF : Low memory (1MB)
0x00100000 - 0x003FFFFF : Kernel code/data (loaded at 1MB)
0xC0000000 - 0xFFFFFFFF : Kernel virtual address space
0xC0400000 - 0xC07FFFFF : Kernel heap (4MB)
```

### Security Model

1. **Privilege Levels**:
   - Ring 0: Kernel mode (full access)
   - Ring 3: User mode (restricted access)

2. **Memory Protection**:
   - All user pointers validated before use
   - Kernel memory inaccessible from user mode
   - Page-level protection enforced by CPU

3. **Input Validation**:
   - All system call parameters validated
   - Strings sanitized to prevent injection
   - Buffer sizes checked to prevent overflows

4. **Stack Protection**:
   - Stack canaries detect buffer overflows
   - Separate kernel stacks per process

## File Structure

```
project/
├── boot.asm          - Bootloader and GDT/IDT flush routines
├── isr.asm           - Interrupt service routine stubs
├── kernel.c          - Main kernel and initialization
├── kernel.h          - Kernel headers and definitions
├── memory.c/h        - Memory management subsystem
├── interrupts.c/h    - Interrupt handling
├── vga.c/h           - VGA text mode driver
├── keyboard.c/h      - PS/2 keyboard driver
├── security.c/h      - Security features
├── process.c/h       - Process management
├── syscall.c/h       - System call interface
├── string.c/h        - String utilities
├── linker.ld         - Linker script
├── Makefile          - Build system
└── grub.cfg          - GRUB configuration
```

## Development

### Adding New Features

1. **New System Call**:
   - Add syscall number to `syscall.h`
   - Implement handler in `syscall.c`
   - Add validation logic

2. **New Driver**:
   - Create driver files (driver.c/h)
   - Initialize in `kernel_main()`
   - Register interrupt handlers if needed

3. **New Command**:
   - Add command handling in `shell_command()` in `kernel.c`

### Debugging

Use QEMU with GDB:

```bash
qemu-system-i386 -cdrom kernel.iso -s -S
```

In another terminal:
```bash
gdb kernel.bin
(gdb) target remote localhost:1234
(gdb) continue
```

## Security Considerations

This kernel implements several security features, but it's designed for **educational purposes only**. For production use, you would need:

- ASLR (Address Space Layout Randomization)
- DEP/NX (Data Execution Prevention)
- Secure boot
- Cryptographic verification
- More robust input validation
- Comprehensive error handling
- Security auditing and logging
- Protection against timing attacks
- Secure random number generation

## Known Limitations

- Single-core only (no SMP support)
- Limited to 32-bit x86 architecture
- Basic round-robin scheduler (no priority scheduling)
- No filesystem support
- No network stack
- Limited driver support
- Fixed 32MB RAM assumption

## Contributing

This is an educational project. Feel free to:
- Study the code
- Modify and experiment
- Use as a learning resource
- Build upon it for your own projects

## License

This kernel is provided as-is for educational purposes.

## Resources

- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development resource
- [Intel Software Developer Manuals](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [Multiboot Specification](https://www.gnu.org/software/grub/manual/multiboot/multiboot.html)

## Acknowledgments

Built with reference to various OS development tutorials and the OSDev community.

---

**LainKernel v1.0** - A demonstration of fundamental operating system concepts with security in mind.
