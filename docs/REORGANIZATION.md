# Reorganização do Projeto - Estrutura de Diretórios

## 📁 Estrutura Recomendada

```
project/
├── src/
│   ├── kernel/
│   │   ├── kernel.c
│   │   ├── kernel.h
│   │   └── panic.c
│   ├── mm/                    (Memory Management)
│   │   ├── memory.c
│   │   ├── memory.h
│   │   ├── paging.c
│   │   └── heap.c
│   ├── arch/
│   │   └── x86/
│   │       ├── boot.asm
│   │       ├── gdt.c
│   │       ├── idt.c
│   │       └── tss.c
│   ├── interrupts/
│   │   ├── interrupts.c
│   │   ├── interrupts.h
│   │   └── isr.asm
│   ├── drivers/
│   │   ├── vga.c
│   │   ├── vga.h
│   │   ├── keyboard.c
│   │   └── keyboard.h
│   ├── security/
│   │   ├── security.c
│   │   ├── security.h
│   │   ├── random.c
│   │   ├── random.h
│   │   ├── audit.c
│   │   └── audit.h
│   ├── process/
│   │   ├── process.c
│   │   ├── process.h
│   │   ├── scheduler.c
│   │   └── syscall.c
│   ├── lib/
│   │   ├── string.c
│   │   └── string.h
│   └── shell/
│       └── shell.c
├── include/
│   └── kernel/
│       ├── types.h
│       ├── config.h
│       └── common.h
├── build/
│   └── (arquivos .o vão aqui)
├── iso/
│   └── boot/
│       └── grub/
│           └── grub.cfg
├── docs/
│   ├── README.md
│   ├── SECURITY_ENHANCEMENTS.md
│   ├── SUGGESTIONS.md
│   └── ARCHITECTURE.md
├── scripts/
│   ├── build.sh
│   ├── run.sh
│   └── clean.sh
├── Makefile
└── linker.ld
```

## 🎯 Organização por Módulo

### 1. **src/kernel/** - Núcleo do Kernel
- `kernel.c/h` - Entry point e inicialização
- `panic.c` - Kernel panic handler

### 2. **src/mm/** - Gerenciamento de Memória
- `memory.c/h` - Interface principal
- `paging.c` - Sistema de paginação
- `heap.c` - Alocador de heap

### 3. **src/arch/x86/** - Código Específico da Arquitetura
- `boot.asm` - Bootloader
- `gdt.c` - Global Descriptor Table
- `idt.c` - Interrupt Descriptor Table
- `tss.c` - Task State Segment

### 4. **src/interrupts/** - Sistema de Interrupções
- `interrupts.c/h` - Gerenciamento de interrupções
- `isr.asm` - Assembly stubs

### 5. **src/drivers/** - Drivers de Hardware
- `vga.c/h` - Driver VGA
- `keyboard.c/h` - Driver de teclado
- (futuro: `ata.c`, `rtl8139.c`, etc)

### 6. **src/security/** - Segurança
- `security.c/h` - Funções de segurança
- `random.c/h` - CSPRNG
- `audit.c/h` - Sistema de auditoria

### 7. **src/process/** - Gerenciamento de Processos
- `process.c/h` - PCB e gerenciamento
- `scheduler.c` - Escalonador
- `syscall.c/h` - System calls

### 8. **src/lib/** - Biblioteca Padrão
- `string.c/h` - Funções de string
- (futuro: `stdio.c`, `stdlib.c`, etc)

### 9. **src/shell/** - Interface de Usuário
- `shell.c` - Shell interativo

### 10. **include/kernel/** - Headers Globais
- `types.h` - Definições de tipos
- `config.h` - Configurações do kernel
- `common.h` - Definições comuns

## 📝 Makefile Atualizado

```makefile
# Diretórios
SRC_DIR = src
BUILD_DIR = build
ISO_DIR = iso
INCLUDE_DIR = include

# Compilador
AS = nasm
CC = gcc
LD = ld

# Flags
ASFLAGS = -f elf32
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -I$(INCLUDE_DIR) \
         -fno-exceptions -fno-stack-protector -nostdlib -nostdinc -fno-builtin
LDFLAGS = -m elf_i386 -T linker.ld -nostdlib

# Encontrar todos os arquivos
ASM_SOURCES = $(shell find $(SRC_DIR) -name '*.asm')
C_SOURCES = $(shell find $(SRC_DIR) -name '*.c')

# Objetos
ASM_OBJECTS = $(patsubst $(SRC_DIR)/%.asm,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
C_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
OBJECTS = $(ASM_OBJECTS) $(C_OBJECTS)

# Output
KERNEL = $(BUILD_DIR)/kernel.bin
ISO = kernel.iso

.PHONY: all clean run dirs

all: dirs $(ISO)

dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BUILD_DIR)/kernel
	@mkdir -p $(BUILD_DIR)/mm
	@mkdir -p $(BUILD_DIR)/arch/x86
	@mkdir -p $(BUILD_DIR)/interrupts
	@mkdir -p $(BUILD_DIR)/drivers
	@mkdir -p $(BUILD_DIR)/security
	@mkdir -p $(BUILD_DIR)/process
	@mkdir -p $(BUILD_DIR)/lib
	@mkdir -p $(BUILD_DIR)/shell
	@mkdir -p $(ISO_DIR)/boot/grub

$(ISO): $(KERNEL)
	@cp $(KERNEL) $(ISO_DIR)/boot/kernel.bin
	@cp iso/boot/grub/grub.cfg $(ISO_DIR)/boot/grub/
	@grub-mkrescue -o $(ISO) $(ISO_DIR) 2>/dev/null || echo "GRUB not found"

$(KERNEL): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.asm
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO)

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO)
```

## 🔄 Script de Migração

Crie `scripts/reorganize.sh`:

```bash
#!/bin/bash

echo "Reorganizando projeto..."

# Criar estrutura de diretórios
mkdir -p src/{kernel,mm,arch/x86,interrupts,drivers,security,process,lib,shell}
mkdir -p include/kernel
mkdir -p build
mkdir -p iso/boot/grub
mkdir -p docs
mkdir -p scripts

# Mover arquivos do kernel
mv kernel.c src/kernel/
mv kernel.h include/kernel/

# Mover memory management
mv memory.c src/mm/
mv memory.h src/mm/

# Mover arch-specific
mv boot.asm src/arch/x86/
mv linker.ld .

# Mover interrupts
mv interrupts.c src/interrupts/
mv interrupts.h src/interrupts/
mv isr.asm src/interrupts/

# Mover drivers
mv vga.c src/drivers/
mv vga.h src/drivers/
mv keyboard.c src/drivers/
mv keyboard.h src/drivers/

# Mover security
mv security.c src/security/
mv security.h src/security/
mv random.c src/security/
mv random.h src/security/
mv audit.c src/security/
mv audit.h src/security/

# Mover process
mv process.c src/process/
mv process.h src/process/
mv syscall.c src/process/
mv syscall.h src/process/

# Mover lib
mv string.c src/lib/
mv string.h src/lib/

# Mover docs
mv README.md docs/
mv SECURITY_ENHANCEMENTS.md docs/
mv SUGGESTIONS.md docs/

# Mover GRUB config
mv grub.cfg iso/boot/grub/

echo "Reorganização completa!"
echo "Agora atualize os #include nos arquivos .c"
```

## 🔧 Atualizar Includes

Depois de reorganizar, atualizar os includes:

**Antes**:
```c
#include "kernel.h"
#include "memory.h"
```

**Depois**:
```c
#include <kernel/kernel.h>
#include <kernel/memory.h>
```

Ou criar um `include/kernel/common.h`:
```c
#ifndef COMMON_H
#define COMMON_H

#include <kernel/types.h>
#include <kernel/kernel.h>
#include <kernel/memory.h>
// ... outros includes comuns

#endif
```

E usar:
```c
#include <kernel/common.h>
```

## 🎨 Estrutura Alternativa (Mais Simples)

Se preferir algo mais simples:

```
project/
├── kernel/
│   ├── core/          (kernel.c, panic.c)
│   ├── mm/            (memory.c, paging.c)
│   ├── arch/          (boot.asm, gdt.c, idt.c)
│   ├── drivers/       (vga.c, keyboard.c)
│   ├── security/      (security.c, random.c, audit.c)
│   └── process/       (process.c, syscall.c)
├── lib/               (string.c)
├── include/           (todos os .h)
├── build/
├── docs/
└── Makefile
```

## ✅ Vantagens da Organização

1. **Escalabilidade** - Fácil adicionar novos módulos
2. **Manutenção** - Código relacionado fica junto
3. **Compilação** - Build incremental mais rápido
4. **Profissionalismo** - Estrutura padrão da indústria
5. **Colaboração** - Outros desenvolvedores entendem rápido

## 🚀 Próximos Passos

1. Execute o script de reorganização
2. Atualize os includes nos arquivos
3. Teste a compilação: `make clean && make`
4. Verifique se tudo funciona: `make run`

Quer que eu crie os scripts e ajude na migração?
