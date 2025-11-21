# Opções de Implementações e Melhorias de Segurança

Este documento apresenta opções de implementações adicionais e melhorias de segurança que podem ser adicionadas ao SecureOS Kernel.

---

## 🔐 Melhorias de Segurança Críticas

### 1. ASLR (Address Space Layout Randomization)

**Descrição**: Randomiza o endereço de carregamento do kernel e das estruturas de dados para dificultar ataques de exploração.

**Implementação**:
```c
// Em memory.c
#define ASLR_ENTROPY_BITS 8

static u32 get_random_offset(void) {
    // Usar RDTSC (timestamp counter) como fonte de entropia
    u32 low, high;
    __asm__ volatile("rdtsc" : "=a"(low), "=d"(high));
    return (low ^ high) & ((1 << ASLR_ENTROPY_BITS) - 1);
}

void aslr_randomize_kernel_base(void) {
    u32 random_offset = get_random_offset() * PAGE_SIZE;
    // Ajustar base do kernel com offset aleatório
}
```

**Benefícios**:
- ✅ Dificulta ataques de buffer overflow
- ✅ Protege contra ROP (Return-Oriented Programming)
- ✅ Aumenta complexidade de exploits

---

### 2. DEP/NX (Data Execution Prevention / No-Execute)

**Descrição**: Marca páginas de memória como não-executáveis para prevenir execução de código em áreas de dados.

**Implementação**:
```c
// Em memory.h
#define PAGE_NX (1 << 31)  // Bit NX em PAE mode

// Em memory.c
void paging_set_nx_bit(page_t* page, bool nx) {
    if (nx) {
        page->frame |= PAGE_NX;
    } else {
        page->frame &= ~PAGE_NX;
    }
}

// Marcar stack e heap como não-executáveis
void memory_protect_data_pages(void) {
    for (u32 addr = KERNEL_HEAP_START; addr < heap_end; addr += PAGE_SIZE) {
        page_t* page = paging_get_page(addr, false, kernel_directory);
        if (page) {
            paging_set_nx_bit(page, true);
        }
    }
}
```

**Benefícios**:
- ✅ Previne shellcode injection
- ✅ Bloqueia execução de código malicioso em stack/heap
- ✅ Proteção em nível de hardware

---

### 3. Gerador de Números Aleatórios Criptograficamente Seguro

**Descrição**: Implementar CSPRNG (Cryptographically Secure Pseudo-Random Number Generator) para canários e ASLR.

**Implementação**:
```c
// Novo arquivo: random.c/h
typedef struct {
    u32 state[4];
    u32 counter;
} prng_state_t;

static prng_state_t prng_state;

void random_init(void) {
    // Seed com múltiplas fontes de entropia
    u32 rdtsc_low, rdtsc_high;
    __asm__ volatile("rdtsc" : "=a"(rdtsc_low), "=d"(rdtsc_high));
    
    prng_state.state[0] = rdtsc_low;
    prng_state.state[1] = rdtsc_high;
    prng_state.state[2] = inb(0x40);  // PIT timer
    prng_state.state[3] = inb(0x61);  // System control port
    prng_state.counter = 0;
}

u32 random_get(void) {
    // Implementar ChaCha20 ou similar
    // Por simplicidade, usar Xorshift128+
    u64 s1 = prng_state.state[0];
    u64 s0 = prng_state.state[1];
    prng_state.state[0] = s0;
    s1 ^= s1 << 23;
    prng_state.state[1] = s1 ^ s0 ^ (s1 >> 18) ^ (s0 >> 5);
    return prng_state.state[1] + s0;
}

// Usar em security.c
void security_init(void) {
    random_init();
    stack_canary = random_get();
}
```

**Benefícios**:
- ✅ Canários imprevisíveis
- ✅ ASLR mais efetivo
- ✅ Geração de IDs seguros

---

### 4. Proteção contra Time-of-Check to Time-of-Use (TOCTOU)

**Descrição**: Prevenir race conditions em validações de segurança.

**Implementação**:
```c
// Em syscall.c
static bool safe_copy_from_user(void* kernel_dest, const void* user_src, u32 size) {
    // Validar ponteiro
    if (!memory_validate_user_ptr(user_src, size)) {
        return false;
    }
    
    // Desabilitar interrupções durante cópia
    __asm__ volatile("cli");
    
    // Re-validar (prevenir TOCTOU)
    if (!memory_validate_user_ptr(user_src, size)) {
        __asm__ volatile("sti");
        return false;
    }
    
    // Copiar atomicamente
    memcpy(kernel_dest, user_src, size);
    
    __asm__ volatile("sti");
    return true;
}
```

**Benefícios**:
- ✅ Previne race conditions
- ✅ Garante atomicidade de operações críticas
- ✅ Evita exploits de timing

---

### 5. Auditoria e Logging de Segurança

**Descrição**: Sistema de logging para rastrear eventos de segurança.

**Implementação**:
```c
// Novo arquivo: audit.c/h
typedef enum {
    AUDIT_SYSCALL,
    AUDIT_PRIVILEGE_VIOLATION,
    AUDIT_MEMORY_VIOLATION,
    AUDIT_INVALID_POINTER,
    AUDIT_STACK_SMASH,
    AUDIT_PROCESS_CREATE,
    AUDIT_PROCESS_TERMINATE
} audit_event_type_t;

typedef struct {
    u32 timestamp;
    audit_event_type_t type;
    u32 pid;
    u32 data[4];
} audit_log_entry_t;

#define AUDIT_LOG_SIZE 1024
static audit_log_entry_t audit_log[AUDIT_LOG_SIZE];
static u32 audit_log_index = 0;

void audit_log_event(audit_event_type_t type, u32 data0, u32 data1, u32 data2, u32 data3) {
    audit_log_entry_t* entry = &audit_log[audit_log_index];
    entry->timestamp = get_tick_count();
    entry->type = type;
    entry->pid = process_get_current() ? process_get_current()->pid : 0;
    entry->data[0] = data0;
    entry->data[1] = data1;
    entry->data[2] = data2;
    entry->data[3] = data3;
    
    audit_log_index = (audit_log_index + 1) % AUDIT_LOG_SIZE;
}

// Usar em pontos críticos
void syscall_handler(struct registers* regs) {
    audit_log_event(AUDIT_SYSCALL, regs->eax, regs->ebx, regs->ecx, regs->edx);
    // ... resto do código
}
```

**Benefícios**:
- ✅ Rastreabilidade de eventos
- ✅ Detecção de ataques
- ✅ Análise forense

---

### 6. Proteção de Integridade do Kernel (Self-Checking)

**Descrição**: Verificar integridade do código do kernel em runtime.

**Implementação**:
```c
// Novo arquivo: integrity.c/h
static u32 kernel_checksum = 0;

u32 calculate_checksum(void* start, u32 size) {
    u32 sum = 0;
    u8* data = (u8*)start;
    for (u32 i = 0; i < size; i++) {
        sum = (sum << 1) ^ data[i];
    }
    return sum;
}

void integrity_init(void) {
    extern u32 _kernel_start;
    extern u32 _kernel_end;
    u32 size = (u32)&_kernel_end - (u32)&_kernel_start;
    kernel_checksum = calculate_checksum(&_kernel_start, size);
}

bool integrity_check(void) {
    extern u32 _kernel_start;
    extern u32 _kernel_end;
    u32 size = (u32)&_kernel_end - (u32)&_kernel_start;
    u32 current_checksum = calculate_checksum(&_kernel_start, size);
    
    if (current_checksum != kernel_checksum) {
        kernel_panic("Kernel integrity violation detected!");
        return false;
    }
    return true;
}

// Chamar periodicamente via timer
void timer_handler(struct registers* regs) {
    static u32 check_counter = 0;
    if (++check_counter >= 1000) {  // A cada 1000 ticks
        integrity_check();
        check_counter = 0;
    }
}
```

**Benefícios**:
- ✅ Detecta modificações maliciosas
- ✅ Previne rootkits
- ✅ Garante integridade do código

---

### 7. Sandboxing de Processos

**Descrição**: Isolar processos com permissões limitadas.

**Implementação**:
```c
// Em process.h
typedef enum {
    PERM_READ = 1,
    PERM_WRITE = 2,
    PERM_EXEC = 4,
    PERM_NETWORK = 8,
    PERM_FILESYSTEM = 16
} permission_t;

typedef struct process {
    // ... campos existentes
    u32 permissions;
    u32 allowed_syscalls[8];  // Bitmap de syscalls permitidas
} process_t;

// Em syscall.c
bool process_has_permission(process_t* proc, permission_t perm) {
    return (proc->permissions & perm) != 0;
}

bool process_can_syscall(process_t* proc, u32 syscall_num) {
    u32 index = syscall_num / 32;
    u32 bit = syscall_num % 32;
    return (proc->allowed_syscalls[index] & (1 << bit)) != 0;
}

void syscall_handler(struct registers* regs) {
    process_t* current = process_get_current();
    
    if (!process_can_syscall(current, regs->eax)) {
        audit_log_event(AUDIT_PRIVILEGE_VIOLATION, regs->eax, 0, 0, 0);
        return;  // Bloquear syscall não autorizada
    }
    // ... resto do código
}
```

**Benefícios**:
- ✅ Princípio do menor privilégio
- ✅ Limita danos de processos comprometidos
- ✅ Controle granular de permissões

---

### 8. Proteção contra Buffer Overflow (SafeStack)

**Descrição**: Separar stack de variáveis locais e endereços de retorno.

**Implementação**:
```c
// Em process.c
typedef struct process {
    // ... campos existentes
    u32 safe_stack;      // Stack para endereços de retorno
    u32 unsafe_stack;    // Stack para buffers e variáveis
} process_t;

process_t* process_create(void (*entry_point)(void), u8 privilege_level) {
    // ... código existente
    
    // Alocar duas stacks separadas
    proc->safe_stack = (u32)kmalloc(4096) + 4096;
    proc->unsafe_stack = (u32)kmalloc(4096) + 4096;
    
    // Configurar ESP para unsafe stack
    proc->esp = proc->unsafe_stack;
    
    return proc;
}
```

**Benefícios**:
- ✅ Protege endereços de retorno
- ✅ Dificulta ROP attacks
- ✅ Isola dados críticos

---

### 9. Rate Limiting de System Calls

**Descrição**: Limitar taxa de chamadas de sistema para prevenir DoS.

**Implementação**:
```c
// Em syscall.c
#define MAX_SYSCALLS_PER_SECOND 1000

typedef struct {
    u32 last_reset_time;
    u32 call_count;
} syscall_rate_limit_t;

static syscall_rate_limit_t rate_limits[MAX_PROCESSES];

bool check_rate_limit(u32 pid) {
    u32 current_time = get_tick_count();
    syscall_rate_limit_t* limit = &rate_limits[pid];
    
    // Reset a cada segundo
    if (current_time - limit->last_reset_time >= 1000) {
        limit->call_count = 0;
        limit->last_reset_time = current_time;
    }
    
    if (limit->call_count >= MAX_SYSCALLS_PER_SECOND) {
        audit_log_event(AUDIT_PRIVILEGE_VIOLATION, pid, limit->call_count, 0, 0);
        return false;  // Rate limit excedido
    }
    
    limit->call_count++;
    return true;
}

void syscall_handler(struct registers* regs) {
    process_t* current = process_get_current();
    
    if (!check_rate_limit(current->pid)) {
        return;  // Bloquear por rate limit
    }
    // ... resto do código
}
```

**Benefícios**:
- ✅ Previne DoS attacks
- ✅ Limita uso abusivo de recursos
- ✅ Detecta comportamento anômalo

---

### 10. Criptografia de Memória Sensível

**Descrição**: Criptografar dados sensíveis em memória.

**Implementação**:
```c
// Novo arquivo: crypto.c/h
typedef struct {
    u32 key[4];
} crypto_key_t;

static crypto_key_t memory_key;

void crypto_init(void) {
    // Gerar chave aleatória
    for (int i = 0; i < 4; i++) {
        memory_key.key[i] = random_get();
    }
}

void crypto_encrypt_buffer(void* buffer, u32 size) {
    u32* data = (u32*)buffer;
    u32 blocks = size / 4;
    
    for (u32 i = 0; i < blocks; i++) {
        // XOR simples (usar AES em produção)
        data[i] ^= memory_key.key[i % 4];
    }
}

void crypto_decrypt_buffer(void* buffer, u32 size) {
    crypto_encrypt_buffer(buffer, size);  // XOR é simétrico
}

// Usar para proteger dados sensíveis
void process_store_credentials(process_t* proc, const char* password) {
    strcpy(proc->credentials, password);
    crypto_encrypt_buffer(proc->credentials, strlen(password));
}
```

**Benefícios**:
- ✅ Protege dados em memória
- ✅ Dificulta memory dumps
- ✅ Segurança adicional para credenciais

---

## 🚀 Implementações Funcionais Adicionais

### 11. Sistema de Arquivos Virtual (VFS)

**Descrição**: Camada de abstração para sistemas de arquivos.

**Estrutura**:
```c
// Novo arquivo: vfs.c/h
typedef struct vfs_node {
    char name[128];
    u32 inode;
    u32 size;
    u32 flags;
    u32 (*read)(struct vfs_node*, u32 offset, u32 size, u8* buffer);
    u32 (*write)(struct vfs_node*, u32 offset, u32 size, u8* buffer);
    void (*open)(struct vfs_node*);
    void (*close)(struct vfs_node*);
    struct vfs_node* next;
} vfs_node_t;

vfs_node_t* vfs_root = NULL;

vfs_node_t* vfs_open(const char* path);
u32 vfs_read(vfs_node_t* node, u32 offset, u32 size, u8* buffer);
u32 vfs_write(vfs_node_t* node, u32 offset, u32 size, u8* buffer);
```

---

### 12. Timer e Scheduler Preemptivo

**Descrição**: Implementar preempção baseada em timer.

**Implementação**:
```c
// Em interrupts.c
#define PIT_FREQUENCY 100  // 100 Hz

void timer_init(void) {
    u32 divisor = 1193180 / PIT_FREQUENCY;
    outb(0x43, 0x36);
    outb(0x40, divisor & 0xFF);
    outb(0x40, (divisor >> 8) & 0xFF);
    
    register_interrupt_handler(32, timer_handler);
}

void timer_handler(struct registers* regs) {
    static u32 tick = 0;
    tick++;
    
    // Preempção a cada 10ms (10 ticks)
    if (tick % 10 == 0) {
        process_schedule();
    }
}
```

---

### 13. Suporte a Multithreading

**Descrição**: Threads dentro de processos.

**Estrutura**:
```c
typedef struct thread {
    u32 tid;
    u32 esp;
    u32 eip;
    u32 state;
    struct thread* next;
} thread_t;

typedef struct process {
    // ... campos existentes
    thread_t* threads;
    u32 thread_count;
} process_t;
```

---

### 14. Sistema de Sinais (Signals)

**Descrição**: Comunicação entre processos via sinais.

**Implementação**:
```c
#define SIGKILL 9
#define SIGTERM 15
#define SIGUSR1 10

typedef void (*signal_handler_t)(int);

typedef struct process {
    // ... campos existentes
    signal_handler_t signal_handlers[32];
} process_t;

void process_send_signal(u32 pid, int signal);
void process_handle_signals(process_t* proc);
```

---

### 15. Proteção de Memória por Segmento

**Descrição**: Usar segmentação adicional para proteção.

**Implementação**:
```c
void gdt_set_segment_limit(int num, u32 limit) {
    gdt_entries[num].limit_low = limit & 0xFFFF;
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
}

// Limitar acesso de processos a regiões específicas
void process_set_memory_limit(process_t* proc, u32 limit) {
    gdt_set_segment_limit(3, limit);  // User code segment
    gdt_set_segment_limit(4, limit);  // User data segment
}
```

---

## 📊 Priorização de Implementações

### Prioridade Alta (Segurança Crítica)
1. ✅ **ASLR** - Dificulta exploits significativamente
2. ✅ **DEP/NX** - Proteção fundamental contra code injection
3. ✅ **CSPRNG** - Base para outras features de segurança
4. ✅ **Auditoria** - Essencial para detecção de ataques

### Prioridade Média (Segurança Importante)
5. ✅ **TOCTOU Protection** - Previne race conditions
6. ✅ **Sandboxing** - Limita danos de comprometimento
7. ✅ **Rate Limiting** - Previne DoS
8. ✅ **Integrity Checking** - Detecta modificações

### Prioridade Baixa (Features Adicionais)
9. ✅ **SafeStack** - Proteção adicional contra overflow
10. ✅ **Memory Encryption** - Para dados muito sensíveis

---

## 🛠️ Próximos Passos Recomendados

1. **Implementar ASLR e DEP/NX** primeiro (maior impacto em segurança)
2. **Adicionar CSPRNG** para melhorar canários e ASLR
3. **Implementar sistema de auditoria** para monitoramento
4. **Adicionar timer preemptivo** para melhor scheduling
5. **Implementar VFS** para suporte a arquivos

---

**Nota**: Todas essas implementações podem ser adicionadas incrementalmente ao kernel existente sem quebrar funcionalidades atuais.
