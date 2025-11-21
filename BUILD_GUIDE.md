# 🔨 LainKernel - Build Guide

Complete guide to building and running LainKernel on your system.

---

## 📋 Table of Contents

- [Prerequisites](#prerequisites)
- [Quick Start](#quick-start)
- [Detailed Build Process](#detailed-build-process)
- [Running the Kernel](#running-the-kernel)
- [Make Commands Explained](#make-commands-explained)
- [Troubleshooting](#troubleshooting)
- [Windows-Specific Instructions](#windows-specific-instructions)

---

## 🛠️ Prerequisites

Before building LainKernel, you need to install the following tools:

### Required Tools

| Tool | Purpose | Download Link |
|------|---------|---------------|
| **GCC Cross-Compiler** | Compile C code for x86 | [OSDev Wiki Guide](https://wiki.osdev.org/GCC_Cross-Compiler) |
| **NASM** | Assemble x86 assembly code | [nasm.us](https://www.nasm.us/) |
| **GNU Make** | Build automation | [GnuWin32](http://gnuwin32.sourceforge.net/packages/make.htm) |
| **GRUB** | Bootloader | [GNU GRUB](https://www.gnu.org/software/grub/) |
| **QEMU** | x86 emulator for testing | [qemu.org](https://www.qemu.org/download/) |

### Verifying Installation

Run these commands to verify everything is installed:

```bash
# Check GCC cross-compiler
i686-elf-gcc --version

# Check NASM
nasm --version

# Check Make
make --version

# Check QEMU
qemu-system-i386 --version
```

> ⚠️ **Important**: You need a **cross-compiler** (i686-elf-gcc), not your system's regular gcc!

---

## ⚡ Quick Start

If you have all prerequisites installed:

```bash
# 1. Navigate to project directory
cd c:\Users\Gabriel\Documents\project

# 2. Build the kernel
make

# 3. Run in QEMU
make run
```

That's it! The kernel should boot in QEMU.

---

## 📚 Detailed Build Process

### Step 1: Clean Previous Builds (Optional)

```bash
make clean
```

**What it does:**
- Deletes the `build/` directory (all compiled `.o` files)
- Deletes `lainkernel.iso` (bootable ISO file)
- Ensures a fresh build from scratch

**When to use:**
- After making changes to header files
- When you get weird compilation errors
- Before committing to Git (to avoid committing binaries)

---

### Step 2: Build the Kernel

```bash
make
```

**What it does:**

1. **Creates build directories** (`build/kernel/`, `build/mm/`, etc.)
2. **Assembles ASM files**:
   - `src/arch/x86/boot.asm` → `build/arch/x86/boot.o`
   - `src/interrupts/isr.asm` → `build/interrupts/isr.o`
3. **Compiles C files**:
   - `src/kernel/kernel.c` → `build/kernel/kernel.o`
   - `src/mm/memory.c` → `build/mm/memory.o`
   - ... (all other `.c` files)
4. **Links all object files** into `build/kernel.bin`
5. **Creates bootable ISO** using GRUB:
   - Copies `kernel.bin` to `iso/boot/`
   - Runs `grub-mkrescue` to create `lainkernel.iso`

**Output:**
```
nasm -f elf32 src/arch/x86/boot.asm -o build/arch/x86/boot.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -Iinclude -c src/kernel/kernel.c -o build/kernel/kernel.o
...
ld -m elf_i386 -T linker.ld -nostdlib -o build/kernel.bin <all .o files>
grub-mkrescue -o lainkernel.iso iso
```

**Build time:** ~10-30 seconds (depending on your system)

---

### Step 3: Run the Kernel

```bash
make run
```

**What it does:**
- Automatically builds the kernel (if not already built)
- Launches QEMU with the bootable ISO
- Opens a window showing the kernel running

**QEMU command executed:**
```bash
qemu-system-i386 -cdrom lainkernel.iso
```

---

## 🎮 Running the Kernel

### Method 1: Using Make (Recommended)

```bash
make run
```

### Method 2: Manual QEMU

```bash
qemu-system-i386 -cdrom lainkernel.iso
```

### Method 3: Advanced QEMU Options

```bash
# With more memory (128 MB)
qemu-system-i386 -cdrom lainkernel.iso -m 128

# With serial output for debugging
qemu-system-i386 -cdrom lainkernel.iso -serial stdio

# With GDB debugging support
qemu-system-i386 -cdrom lainkernel.iso -s -S
```

### Method 4: Real Hardware (Advanced)

> ⚠️ **Warning**: Only do this if you know what you're doing!

1. Write ISO to USB drive:
   ```bash
   # Linux
   sudo dd if=lainkernel.iso of=/dev/sdX bs=4M
   
   # Windows (use Rufus or similar tool)
   ```

2. Boot from USB on a real x86 computer

---

## 📖 Make Commands Explained

### `make` or `make all`
- **Purpose**: Build everything
- **Creates**: `lainkernel.iso`
- **Use when**: You want to compile the kernel

### `make clean`
- **Purpose**: Remove all build artifacts
- **Deletes**: `build/` directory and `lainkernel.iso`
- **Use when**: Starting fresh or before committing to Git

### `make run`
- **Purpose**: Build and run in QEMU
- **Requires**: QEMU installed
- **Use when**: Testing your kernel

### `make dirs`
- **Purpose**: Create build directory structure
- **Use when**: Rarely needed (called automatically by `make`)

---

## 🐛 Troubleshooting

### Error: `make: command not found`

**Problem**: Make is not installed or not in PATH

**Solution**:
- **Windows**: Install [GnuWin32 Make](http://gnuwin32.sourceforge.net/packages/make.htm)
- **Linux**: `sudo apt install build-essential`
- **macOS**: `xcode-select --install`

---

### Error: `i686-elf-gcc: command not found`

**Problem**: Cross-compiler not installed

**Solution**: Build a cross-compiler following [OSDev Wiki Guide](https://wiki.osdev.org/GCC_Cross-Compiler)

**Quick check**: Are you using `i686-elf-gcc` (cross-compiler) or just `gcc` (system compiler)?

---

### Error: `nasm: command not found`

**Problem**: NASM assembler not installed

**Solution**:
- Download from [nasm.us](https://www.nasm.us/)
- Add to PATH

---

### Error: `grub-mkrescue: command not found`

**Problem**: GRUB not installed

**Solution**:
- **Windows**: Install GRUB for Windows or use WSL
- **Linux**: `sudo apt install grub-pc-bin xorriso`
- **macOS**: `brew install grub xorriso`

---

### Error: Kernel doesn't boot in QEMU

**Symptoms**: Black screen, immediate crash, or reboot loop

**Solutions**:

1. **Check boot.asm**: Ensure multiboot header is correct
2. **Check linker.ld**: Verify memory addresses
3. **Enable QEMU logging**:
   ```bash
   qemu-system-i386 -cdrom lainkernel.iso -d int,cpu_reset -D qemu.log
   ```
4. **Check GRUB config**: Verify `iso/boot/grub/grub.cfg`

---

### Build succeeds but nothing happens

**Problem**: QEMU not installed or not in PATH

**Solution**:
- Install QEMU from [qemu.org](https://www.qemu.org/download/)
- Add to PATH
- Test: `qemu-system-i386 --version`

---

## 🪟 Windows-Specific Instructions

### Option 1: WSL (Recommended)

Use Windows Subsystem for Linux for easier setup:

```bash
# Install WSL (PowerShell as Admin)
wsl --install

# Inside WSL, install tools
sudo apt update
sudo apt install build-essential nasm grub-pc-bin xorriso qemu-system-x86
```

### Option 2: Native Windows

1. **Install MSYS2**: [msys2.org](https://www.msys2.org/)
2. **Open MSYS2 terminal**:
   ```bash
   pacman -S mingw-w64-x86_64-gcc nasm make grub qemu
   ```

### Option 3: Cygwin

1. Download [Cygwin](https://www.cygwin.com/)
2. During installation, select: gcc, nasm, make, grub, qemu

---

## 🎯 What You Should See

### Successful Build Output

```
Creating build directories...
Assembling boot.asm...
Assembling isr.asm...
Compiling kernel.c...
Compiling memory.c...
...
Linking kernel.bin...
Creating bootable ISO...
Done! lainkernel.iso created successfully.
```

### Successful Kernel Boot

When you run `make run`, QEMU should open and show:

```
LainKernel v0.1.0
Initializing...
[OK] GDT loaded
[OK] IDT loaded
[OK] Memory manager initialized
[OK] Keyboard driver loaded
[OK] Security features enabled

Welcome to LainKernel!
>
```

---

## 📊 Build Process Flowchart

```
┌─────────────┐
│  make clean │ (Optional)
└──────┬──────┘
       │
       ▼
┌─────────────┐
│    make     │
└──────┬──────┘
       │
       ├─► Create build directories
       │
       ├─► Assemble .asm files → .o files
       │
       ├─► Compile .c files → .o files
       │
       ├─► Link all .o files → kernel.bin
       │
       └─► Create ISO with GRUB → lainkernel.iso
       
       ▼
┌─────────────┐
│  make run   │
└──────┬──────┘
       │
       └─► Launch QEMU with lainkernel.iso
```

---

## 🔍 Understanding the Build System

### Directory Structure After Build

```
project/
├── build/              # Compiled object files
│   ├── kernel/
│   │   └── kernel.o
│   ├── mm/
│   │   └── memory.o
│   └── ...
├── iso/                # ISO filesystem
│   └── boot/
│       ├── kernel.bin  # Final kernel binary
│       └── grub/
│           └── grub.cfg
└── lainkernel.iso      # Bootable ISO image
```

### Compilation Flags Explained

**NASM Flags** (`-f elf32`):
- `-f elf32`: Output 32-bit ELF format

**GCC Flags**:
- `-m32`: Compile for 32-bit x86
- `-ffreestanding`: Kernel environment (no standard library)
- `-O2`: Optimization level 2
- `-Wall -Wextra`: Enable all warnings
- `-Iinclude`: Include directory for headers
- `-fno-stack-protector`: Disable stack protection (we implement our own)
- `-nostdlib -nostdinc`: Don't use standard library
- `-fno-builtin`: Don't use built-in functions

**Linker Flags**:
- `-m elf_i386`: Link for 32-bit x86
- `-T linker.ld`: Use custom linker script
- `-nostdlib`: Don't link standard library

---

## 🎓 Next Steps

After successfully building and running LainKernel:

1. **Modify the kernel**: Edit `src/kernel/kernel.c`
2. **Rebuild**: Run `make clean && make`
3. **Test**: Run `make run`
4. **Debug**: Use QEMU with GDB for debugging
5. **Contribute**: Add new features!

---

## 📚 Additional Resources

- [OSDev Wiki](https://wiki.osdev.org/) - Comprehensive OS development guide
- [Intel Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html) - x86 architecture reference
- [GRUB Manual](https://www.gnu.org/software/grub/manual/) - Bootloader documentation
- [QEMU Documentation](https://www.qemu.org/docs/master/) - Emulator guide

---

**Happy kernel hacking! 🚀**
