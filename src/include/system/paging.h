#pragma once

#include <runtime.h>
#include <multiboot.h>
#include <cpu/isr.h>

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

typedef struct page
{
    u32 present    : 1;   // Page present in memory
    u32 rw         : 1;   // Read-only if clear, readwrite if set
    u32 user       : 1;   // Supervisor level only if clear
    u32 accessed   : 1;   // Has the page been accessed since last refresh?
    u32 dirty      : 1;   // Has the page been written to since last refresh?
    u32 unused     : 7;   // Amalgamation of unused and reserved bits
    u32 frame      : 20;  // Frame address (shifted right 12 bits)
} page_t;

typedef struct page_table
{
    page_t pages[1024];
} page_table_t;

typedef struct page_directory
{
    page_table_t *tables[1024];

    u32 tablesPhysical[1024];

    u32 physicalAddr;
} page_directory_t;

void paging_setup(u32 memsize);

void switch_page_directory(page_directory_t *dir);

page_t *get_page(u32 address, int make, page_directory_t *dir);

void page_fault(registers_t *regs);

void paging_mark_system(u64 addr);

page_directory_t *clone_directory(page_directory_t *src);