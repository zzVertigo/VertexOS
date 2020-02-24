#pragma once

#include <runtime.h>
#include <system/types/orderedarray.h>

#define KHEAP_START 0xC0000000
#define KHEAP_INITIAL_SIZE 0x100000

#define HEAP_INDEX_SIZE 0x20000
#define HEAP_MAGIC 0x123890AB
#define HEAP_MIN_SIZE 0x70000

typedef struct {
    u32 magic;
    u8 is_hole;
    u32 size;
} header_t;

typedef struct {
    u32 magic;
    header_t *header;
} footer_t;

typedef struct {
    ordered_array_t index;
    u32 start_address;
    u32 end_address;
    u32 max_address;
    u8 supervisor;
    u8 readonly;
} heap_t;

heap_t *create_heap(u32 start, u32 end, u32 max, u8 supervisor, u8 readonly);

void *alloc(u32 size, u8 page_align, heap_t *heap);

void free(void *p, heap_t *heap);
u32 kmalloc_int(u32 sz, s32 align, u32 *phys);
u32 kmalloc_a(u32 sz);
u32 kmalloc_p(u32 sz, u32 *phys);
u32 kmalloc_ap(u32 sz, u32 *phys);
u32 kmalloc(u32 sz);
void kfree(void *p);