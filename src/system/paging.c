#include <system/paging.h>
#include <system/kheap.h>

extern u32 placement_addr;
extern heap_t *kheap;

u32 *frame_allocations;
u32 nframes;

page_directory_t *kernel_directory = 0;
page_directory_t *current_directory = 0;

static void set_frame(u32 addr) {
    u32 frame = FRAME(addr);
    u32 frame_alloc_s = FRAME_SECTION(frame);
    u32 frame_alloc_o = FRAME_OFFSET(frame);
    frame_allocations[frame_alloc_s] |= (1 << frame_alloc_o);
}

static void clear_frame(u32 addr) {
    u32 frame = FRAME(addr);
    u32 frame_alloc_s = FRAME_SECTION(frame);
    u32 frame_alloc_o = FRAME_OFFSET(frame);
    frame_allocations[frame_alloc_s] &= ~(1 << frame_alloc_o);
}

static u32 first_frame() {
    u32 nsections = nframes / FRAME_ALLOCATIONS_SECTION_SIZE;
    for(u32 sec = 0; sec < nsections; sec++)
        if(frame_allocations[sec] != USED_FRAME_SECTION)
            for(u32 idx = 0; idx < FRAME_ALLOCATIONS_SECTION_SIZE; idx++)
                if(!(frame_allocations[sec] & (0x1 << idx)))
                    return (sec*FRAME_ALLOCATIONS_SECTION_SIZE) + idx;
    return nsections * FRAME_ALLOCATIONS_SECTION_SIZE;
}

void alloc_frame(page_t *page, s32 is_super, s32 is_write) {
    if(page->fr != 0)
        return;
    else {
        u32 fframe = first_frame();
        if(fframe == (u32)-1) {
            panic("No free frames availables!");
        } else {
            // Set free frames to the page
            page->pr = PAGE_PRESENT;
            page->rw = (is_write) ? PAGE_READ_WRITE : PAGE_READ_ONLY;
            page->us = (is_super) ? PAGE_SUPERVISOR : PAGE_USER;
            page->fr = fframe;
            // Set new frames as used
            u32 physical_addr = fframe * FRAME_SIZE;
            set_frame(physical_addr);
        }
    }
}

void free_frame(page_t *page) {
    u32 frame;
    if(!(frame=page->fr))
        return; // page doesn't have a frame in first place
    else {
        clear_frame(frame);
        page->fr = 0x0;
    }
}

void paging_test() {
    int* ptr = kmalloc(500);

    printf("[*] Testing kmalloc() - ");

    if (ptr)
        printf("Success! (0x%x - 0x%x)\n", ptr, &ptr);
    else {
        printf("Failed!\n");

        panic("kmalloc test failed!");
    }

    kfree(ptr);
}

static page_table_t *clone_table(page_table_t *src, u32 *physAddr) {
        // Make a new page table, which is page aligned.
    page_table_t *table = (page_table_t*)kmalloc_ap(sizeof(page_table_t), physAddr);
    // Ensure that the new table is blank.
    memset(table, 0, sizeof(page_directory_t));

    // For every entry in the table...
    int i;
    for (i = 0; i < 1024; i++)
    {
        // If the source entry has a frame associated with it...
        if (!src->pages[i].fr)
            continue;
        // Get a new frame.
        alloc_frame(&table->pages[i], 0, 0);
        // Clone the flags from source to destination.
        if (src->pages[i].pr) table->pages[i].pr = 1;
        if (src->pages[i].rw)      table->pages[i].rw = 1;
        if (src->pages[i].us)    table->pages[i].us = 1;
        if (src->pages[i].ac)table->pages[i].ac = 1;
        if (src->pages[i].di)   table->pages[i].di = 1;
        // Physically copy the data across. This function is in process.s.
        copy_page_physical(src->pages[i].fr*0x1000, table->pages[i].fr*0x1000);
    }
    return table;
}

page_directory_t *clone_directory(page_directory_t *src) {
    u32 phys;
    // Make a new page directory and obtain its physical address.
    page_directory_t *dir = (page_directory_t*)kmalloc_ap(sizeof(page_directory_t), &phys);
    // Ensure that it is blank.
    memset(dir, 0, sizeof(page_directory_t));

    // Get the offset of tablesPhysical from the start of the page_directory_t structure.
    u32 offset = (u32)dir->tablesPhysical - (u32)dir;

    // Then the physical address of dir->tablesPhysical is:
    dir->physicalAddr = phys + offset;

    // Go through each page table. If the page table is in the kernel directory, do not make a new copy.
    int i;
    for (i = 0; i < 1024; i++)
    {
        if (!src->tables[i])
            continue;

        if (kernel_directory->tables[i] == src->tables[i])
        {
            // It's in the kernel, so just use the same pointer.
            dir->tables[i] = src->tables[i];
            dir->tablesPhysical[i] = src->tablesPhysical[i];
        }
        else
        {
            // Copy the table.
            u32 phys;
            dir->tables[i] = clone_table(src->tables[i], &phys);
            dir->tablesPhysical[i] = phys | 0x07;
        }
    }
    return dir;
}

void paging_setup(size_t memsize) {
    // Setup frame allocation
    nframes = memsize / 4;
    frame_allocations = (u32*)kmalloc(nframes * 8);
    memset(frame_allocations, 0, nframes * 8);

    // Setup page directory
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    kernel_directory->physicalAddr = kernel_directory->tablesPhysical;

    u32 i = 0;

    // Map heap pages
    for(i = KHEAP_START; i < KHEAP_START + KHEAP_INITIAL_SIZE; i += FRAME_SIZE)
        get_page(i, 1, kernel_directory);

    // Setup identity map
    i = 0;
    while(i < placement_addr + FRAME_SIZE) {
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);
        i += FRAME_SIZE;
    }
    
    // Allocate heap pages
    for(i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += FRAME_SIZE)
        alloc_frame(get_page(i, 1, kernel_directory), 0, 0);

    // Register a new ISR to listen to IRQ 14
    register_interrupt_handler(14, page_fault);
    enable_paging(kernel_directory);
    kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);

    current_directory = clone_directory(kernel_directory);
    enable_paging(current_directory);
}

void enable_paging(page_directory_t *dir) {
    current_directory = dir;
    asm volatile("mov %0, %%cr3" :: "r"(dir->physicalAddr));
    u32 cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Correct code to enable paging
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

page_t *get_page(u32 address, s32 make, page_directory_t *dir) {
    address /= 0x1000; // turn address into an index
    u32 table_idx = address / 1024; // Find page that contains the address
    if(dir->tables[table_idx])
        return &dir->tables[table_idx]->pages[address%1024];
    else if(make) {
        u32 tmp;
        dir->tables[table_idx] = (page_table_t*)kmalloc_ap(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_idx], 0, 0x1000);
        dir->tablesPhysical[table_idx] = tmp | 0x7;
        return &dir->tables[table_idx]->pages[address%1024];
    } else
        return 0;
}

void page_fault(registers_t *regs) {
    // Handle a page fault
    u32 faulting_addr;
    asm volatile("mov %%cr2, %0" : "=r" (faulting_addr));

    // Gracefully print the error
    printf((u8*)"\nPage fault! ( ");
    if(!(regs->errcode & 0x1))
        printf((u8*)"Present ");
    if(regs->errcode & 0x2)
        printf((u8*)"Read-Only ");
    if(regs->errcode & 0x4)
        printf((u8*)"User-Mode ");
    if(regs->errcode & 0x8)
        printf((u8*)"Reserved");
    printf(") at %x\n", faulting_addr);
    panic("Page fault");
}