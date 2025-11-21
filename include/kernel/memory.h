#ifndef MEMORY_H
#define MEMORY_H

#include "kernel.h"

#define PAGE_SIZE 4096
#define PAGE_ALIGN_DOWN(addr) ((addr) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(addr) (((addr) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

#define PAGE_PRESENT    0x1
#define PAGE_WRITE      0x2
#define PAGE_USER       0x4
#define PAGE_ACCESSED   0x20
#define PAGE_DIRTY      0x40

#define KERNEL_HEAP_START 0xC0400000
#define KERNEL_HEAP_SIZE  0x00400000

typedef struct page {
    u32 present    : 1;
    u32 rw         : 1;
    u32 user       : 1;
    u32 accessed   : 1;
    u32 dirty      : 1;
    u32 unused     : 7;
    u32 frame      : 20;
} page_t;

typedef struct page_table {
    page_t pages[1024];
} page_table_t;

typedef struct page_directory {
    page_table_t* tables[1024];
    u32 tablesPhysical[1024];
    u32 physicalAddr;
} page_directory_t;

typedef struct heap_block {
    u32 size;
    u32 magic;
    bool used;
    struct heap_block* next;
    struct heap_block* prev;
} heap_block_t;

void memory_init(u32 mem_size);
void paging_init(void);

u32 pmm_alloc_frame(void);
void pmm_free_frame(u32 frame_addr);

page_directory_t* paging_get_kernel_directory(void);
page_directory_t* paging_clone_directory(page_directory_t* src);
void paging_switch_directory(page_directory_t* dir);
page_t* paging_get_page(u32 address, bool make, page_directory_t* dir);
void paging_map_page(page_t* page, u32 frame, bool is_kernel, bool is_writeable);
void paging_unmap_page(page_t* page);

void* kmalloc(u32 size);
void* kmalloc_a(u32 size);
void* kmalloc_ap(u32 size, u32* phys);
void kfree(void* ptr);

bool memory_validate_user_ptr(const void* ptr, u32 size);
bool memory_validate_user_string(const char* str, u32 max_len);
bool memory_is_kernel_address(u32 addr);

#endif // MEMORY_H
