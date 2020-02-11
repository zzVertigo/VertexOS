#pragma once

#include <runtime.h>
#include <types/orderedarray.h>

#define KHEAP_START         0xC0000000
#define KHEAP_INITIAL_SIZE  0x100000

#define HEAP_INDEX_SIZE   0x20000
#define HEAP_MAGIC        0x123890AB
#define HEAP_MIN_SIZE     0x70000

typedef struct
{
    u32 magic;   // Magic number, used for error checking and identification.
    u8 is_hole;   // 1 if this is a hole. 0 if this is a block.
    u32 size;    // size of the block, including the end footer.
} header_t;

typedef struct
{
    u32 magic;     // Magic number, same as in header_t.
    header_t *header; // Pointer to the block header.
} footer_t;

typedef struct
{
    ordered_array_t index;
    u32 start_address; // The start of our allocated space.
    u32 end_address;   // The end of our allocated space. May be expanded up to max_address.
    u32 max_address;   // The maximum address the heap can be expanded to.
    u8 supervisor;     // Should extra pages requested by us be mapped as supervisor-only?
    u8 readonly;       // Should extra pages requested by us be mapped as read-only?
} heap_t;

heap_t *create_heap(u32 start, u32 end, u32 max, u8 supervisor, u8 readonly);

void *alloc(u32 size, u8 page_align, heap_t *heap);

void free(void *p, heap_t *heap);

u32 kmalloc_int(u32 sz, int align, u32 *phys);

u32 kmalloc_a(u32 sz);

u32 kmalloc_p(u32 sz, u32 *phys);

u32 kmalloc_ap(u32 sz, u32 *phys);

u32 kmalloc(u32 sz);

void kfree(void *p);