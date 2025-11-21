#include <kernel/memory.h>
#include "../lib/string.h"
#include "../drivers/vga.h"

static u32 total_frames;
static u32* frame_bitmap;
static u32 next_free_frame = 0;

static page_directory_t* kernel_directory = 0;
static page_directory_t* current_directory = 0;

static heap_block_t* heap_start = 0;
static u32 heap_end;
static u32 placement_address = 0;

#define HEAP_MAGIC 0xDEADBEEF
#define HEAP_MIN_BLOCK_SIZE 16

#define INDEX_FROM_BIT(a) (a / 32)
#define OFFSET_FROM_BIT(a) (a % 32)

static void set_frame(u32 frame_addr) {
    u32 frame = frame_addr / PAGE_SIZE;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    frame_bitmap[idx] |= (0x1 << off);
}

static void clear_frame(u32 frame_addr) {
    u32 frame = frame_addr / PAGE_SIZE;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    frame_bitmap[idx] &= ~(0x1 << off);
}

static bool test_frame(u32 frame_addr) {
    u32 frame = frame_addr / PAGE_SIZE;
    u32 idx = INDEX_FROM_BIT(frame);
    u32 off = OFFSET_FROM_BIT(frame);
    return (frame_bitmap[idx] & (0x1 << off)) != 0;
}

static u32 first_free_frame(void) {
    for (u32 i = next_free_frame; i < total_frames; i++) {
        u32 idx = INDEX_FROM_BIT(i);
        u32 off = OFFSET_FROM_BIT(i);
        if (!(frame_bitmap[idx] & (0x1 << off))) {
            next_free_frame = i + 1;
            return i;
        }
    }
    

    for (u32 i = 0; i < next_free_frame; i++) {
        u32 idx = INDEX_FROM_BIT(i);
        u32 off = OFFSET_FROM_BIT(i);
        if (!(frame_bitmap[idx] & (0x1 << off))) {
            next_free_frame = i + 1;
            return i;
        }
    }
    
    return (u32)-1;
}

u32 pmm_alloc_frame(void) {
    u32 frame = first_free_frame();
    if (frame == (u32)-1) {
        kernel_panic("Out of physical memory!");
        return 0;
    }
    set_frame(frame * PAGE_SIZE);
    return frame * PAGE_SIZE;
}

void pmm_free_frame(u32 frame_addr) {
    clear_frame(frame_addr);
    u32 frame = frame_addr / PAGE_SIZE;
    if (frame < next_free_frame) {
        next_free_frame = frame;
    }
}


static u32 kmalloc_early(u32 size, bool align, u32* phys) {
    if (align && (placement_address & 0xFFFFF000)) {
        placement_address &= 0xFFFFF000;
        placement_address += 0x1000;
    }
    
    if (phys) {
        *phys = placement_address;
    }
    
    u32 tmp = placement_address;
    placement_address += size;
    return tmp;
}

page_t* paging_get_page(u32 address, bool make, page_directory_t* dir) {
    address /= PAGE_SIZE;
    u32 table_idx = address / 1024;
    
    if (dir->tables[table_idx]) {
        return &dir->tables[table_idx]->pages[address % 1024];
    } else if (make) {
        u32 phys;
        dir->tables[table_idx] = (page_table_t*)kmalloc_early(sizeof(page_table_t), true, &phys);
        memset(dir->tables[table_idx], 0, sizeof(page_table_t));
        dir->tablesPhysical[table_idx] = phys | PAGE_PRESENT | PAGE_WRITE;
        return &dir->tables[table_idx]->pages[address % 1024];
    }
    
    return 0;
}

void paging_map_page(page_t* page, u32 frame, bool is_kernel, bool is_writeable) {
    page->present = 1;
    page->rw = is_writeable ? 1 : 0;
    page->user = is_kernel ? 0 : 1;
    page->frame = frame / PAGE_SIZE;
}

void paging_unmap_page(page_t* page) {
    if (page->present) {
        pmm_free_frame(page->frame * PAGE_SIZE);
        page->present = 0;
    }
}

void paging_switch_directory(page_directory_t* dir) {
    current_directory = dir;
    __asm__ volatile("mov %0, %%cr3" :: "r"(dir->physicalAddr));
}

page_directory_t* paging_get_kernel_directory(void) {
    return kernel_directory;
}

void paging_init(void) {

    u32 phys;
    kernel_directory = (page_directory_t*)kmalloc_early(sizeof(page_directory_t), true, &phys);
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physicalAddr = phys;
    

    for (u32 i = 0; i < 0x400000; i += PAGE_SIZE) {
        page_t* page = paging_get_page(i, true, kernel_directory);
        paging_map_page(page, i, true, true);
    }
    

    paging_switch_directory(kernel_directory);
    
    u32 cr0;
    __asm__ volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000;
    __asm__ volatile("mov %0, %%cr0" :: "r"(cr0));
}

void memory_init(u32 mem_size) {

    extern u32 end;
    placement_address = (u32)&end;
    

    total_frames = mem_size / PAGE_SIZE;
    

    u32 bitmap_size = total_frames / 8;
    frame_bitmap = (u32*)kmalloc_early(bitmap_size, false, 0);
    memset(frame_bitmap, 0, bitmap_size);
    

    for (u32 i = 0; i < placement_address; i += PAGE_SIZE) {
        set_frame(i);
    }
    

    paging_init();
    

    heap_start = (heap_block_t*)KERNEL_HEAP_START;
    heap_end = KERNEL_HEAP_START + KERNEL_HEAP_SIZE;
    

    for (u32 i = KERNEL_HEAP_START; i < heap_end; i += PAGE_SIZE) {
        page_t* page = paging_get_page(i, true, kernel_directory);
        u32 frame = pmm_alloc_frame();
        paging_map_page(page, frame, true, true);
    }
    

    heap_start->size = KERNEL_HEAP_SIZE - sizeof(heap_block_t);
    heap_start->magic = HEAP_MAGIC;
    heap_start->used = false;
    heap_start->next = 0;
    heap_start->prev = 0;
}

void* kmalloc(u32 size) {
    if (!heap_start) {
        return (void*)kmalloc_early(size, false, 0);
    }
    

    if (size < HEAP_MIN_BLOCK_SIZE) {
        size = HEAP_MIN_BLOCK_SIZE;
    }
    size = (size + 3) & ~3;
    

    heap_block_t* current = heap_start;
    while (current) {

        if (current->magic != HEAP_MAGIC) {
            kernel_panic("Heap corruption detected!");
            return 0;
        }
        
        if (!current->used && current->size >= size) {

            if (current->size >= size + sizeof(heap_block_t) + HEAP_MIN_BLOCK_SIZE) {
                heap_block_t* new_block = (heap_block_t*)((u32)current + sizeof(heap_block_t) + size);
                new_block->size = current->size - size - sizeof(heap_block_t);
                new_block->magic = HEAP_MAGIC;
                new_block->used = false;
                new_block->next = current->next;
                new_block->prev = current;
                
                if (current->next) {
                    current->next->prev = new_block;
                }
                current->next = new_block;
                current->size = size;
            }
            
            current->used = true;
            return (void*)((u32)current + sizeof(heap_block_t));
        }
        
        current = current->next;
    }
    
    kernel_panic("Out of heap memory!");
    return 0;
}

void* kmalloc_a(u32 size) {
    if (!heap_start) {
        return (void*)kmalloc_early(size, true, 0);
    }
    

    return kmalloc(size);
}

void* kmalloc_ap(u32 size, u32* phys) {
    if (!heap_start) {
        return (void*)kmalloc_early(size, true, phys);
    }
    
    void* addr = kmalloc_a(size);
    if (phys && current_directory) {
        page_t* page = paging_get_page((u32)addr, false, current_directory);
        if (page) {
            *phys = page->frame * PAGE_SIZE + ((u32)addr & 0xFFF);
        }
    }
    return addr;
}

void kfree(void* ptr) {
    if (!ptr || !heap_start) {
        return;
    }
    
    heap_block_t* block = (heap_block_t*)((u32)ptr - sizeof(heap_block_t));
    

    if (block->magic != HEAP_MAGIC) {
        kernel_panic("Invalid free: corrupted block!");
        return;
    }
    
    if (!block->used) {
        kernel_panic("Double free detected!");
        return;
    }
    
    block->used = false;
    

    if (block->next && !block->next->used) {
        block->size += sizeof(heap_block_t) + block->next->size;
        block->next = block->next->next;
        if (block->next) {
            block->next->prev = block;
        }
    }
    

    if (block->prev && !block->prev->used) {
        block->prev->size += sizeof(heap_block_t) + block->size;
        block->prev->next = block->next;
        if (block->next) {
            block->next->prev = block->prev;
        }
    }
}


bool memory_is_kernel_address(u32 addr) {
    return addr >= KERNEL_VIRTUAL_BASE;
}

bool memory_validate_user_ptr(const void* ptr, u32 size) {
    u32 addr = (u32)ptr;
    

    if (!ptr) {
        return false;
    }
    

    if (memory_is_kernel_address(addr)) {
        return false;
    }
    

    if (addr + size < addr) {
        return false;
    }
    

    if (memory_is_kernel_address(addr + size - 1)) {
        return false;
    }
    

    if (current_directory) {
        for (u32 i = addr; i < addr + size; i += PAGE_SIZE) {
            page_t* page = paging_get_page(i, false, current_directory);
            if (!page || !page->present || !page->user) {
                return false;
            }
        }
    }
    
    return true;
}

bool memory_validate_user_string(const char* str, u32 max_len) {
    if (!memory_validate_user_ptr(str, 1)) {
        return false;
    }
    

    for (u32 i = 0; i < max_len; i++) {
        if (!memory_validate_user_ptr(str + i, 1)) {
            return false;
        }
        if (str[i] == '\0') {
            return true;
        }
    }
    
    return false;
}
