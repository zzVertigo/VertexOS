#pragma once

#include <runtime.h>
#include <system/kheap.h>

#define FRAME_SIZE 4096
#define PAGE_TABLE_SIZE 1024
#define PAGE_DIRECTORY_SIZE 1024
#define PAGE_NOT_PRESENT 0
#define PAGE_PRESENT 1
#define PAGE_READ_ONLY 0
#define PAGE_READ_WRITE 1
#define PAGE_USER 0
#define PAGE_SUPERVISOR 0
#define PAGE_SIZE_4KB 0
#define PAGE_SIZE_4MB 1
// Frames macros
#define FRAME_ALLOCATIONS_SECTION_SIZE 32
#define USED_FRAME_SECTION 0xFFFFFFFF
#define FREE_FRAME_SECTION 0x00000000

#define FRAME(addr) (addr/FRAME_SIZE)
#define FRAME_SECTION(frame) (frame/FRAME_ALLOCATIONS_SECTION_SIZE)
#define FRAME_OFFSET(frame) (frame%FRAME_ALLOCATIONS_SECTION_SIZE)

// Set physical memory to 15 MiB
#define PHYSICAL_MEM_SIZE 0x10000000

struct page { // Single page structure, from intel's developer manual
    u8 pr : 1; // Present: 1 to map 4KB page
    u8 rw : 1; // Read/Write mode
    u8 us : 1; // if 0, user mode access aren't allowed to the page
    u8 pw : 1; // Page-level write through
    u8 pc : 1; // Page-level cache disable
    u8 ac : 1; // 1 if we have accessed 4kb page
    u8 di : 1; // 1 if page has been written(dirty)
    u8 pa : 1; // Unused bit
    u8 gl : 1; // 1 if page is global
    u8 ig : 3; // Unused bit
    u32 fr: 20; // Physical address of frame
} __attribute__((packed));
typedef struct page page_t;

typedef struct page_table {
    page_t pages[PAGE_TABLE_SIZE];
} page_table_t;

typedef struct page_directory {
    page_table_t *tables[PAGE_DIRECTORY_SIZE];
    u32 tablesPhysical[PAGE_DIRECTORY_SIZE];
    u32 physicalAddr;
} page_directory_t;

void paging_setup(size_t memsize);
void paging_test();
// Perform the "enable-paging" operation to the right register
void enable_paging(page_directory_t *dir);
// Retrieve a pointer from the given page
page_t *get_page(u32 address, s32 make, page_directory_t *dir);
// Identity map(phys = virtual addr) to access it as if paging wasn't enabled
void identity_map();
// Delete a frame
void free_frame(page_t *page);
// Allocate a new frame
void alloc_frame(page_t *page, s32 is_super, s32 is_write);
// Page faults handler(ISR recorder)
void page_fault(registers_t *regs);
void mark_system_memory(u32 address);

page_directory_t *clone_directory(page_directory_t *src);
