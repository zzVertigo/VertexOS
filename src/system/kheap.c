#include <system/kheap.h>
#include <system/paging.h>

extern u32 __end;
u32 placement_addr = (u32)&__end;

extern page_directory_t *kernel_directory;
heap_t *kheap = 0;

u32 kmalloc_int(u32 sz, s32 align, u32 *phys) {
    if(kheap != 0) {
        void *addr = alloc(sz, (u8)align, kheap);
        if(phys != 0) {
            page_t *page = get_page((u32)addr, 0, kernel_directory);
            *phys = page->fr*0x1000 + ((u32)addr&0xFFF);
        }
        return (u32)addr;
    } else {
        if(align == 1 && (placement_addr & 0xFFFFF000)) {
            placement_addr &= 0xFFFFF000;
            placement_addr += 0x1000;
        }
        if(phys)
            *phys = placement_addr;
        u32 tmp = placement_addr;
        placement_addr += sz;
        return tmp;
    }
}

void kfree(void *p) {
    free(p, kheap);
}

u32 kmalloc_a(u32 sz) {
    return kmalloc_int(sz, 1, 0);
}

u32 kmalloc_p(u32 sz, u32 *phys) {
    return kmalloc_int(sz, 0, phys);
}

u32 kmalloc_ap(u32 sz, u32 *phys) {
    return kmalloc_int(sz, 1, phys);
}

u32 kmalloc(u32 sz) {
    return kmalloc_int(sz, 0, 0);
}

static void expand(u32 new_size, heap_t *heap) {
    assert(new_size > heap->end_address - heap->start_address);

    // Get nearest page boundary
    if((new_size&0xFFFFF000) != 0) {
        new_size &= 0xFFFFF000;
        new_size += 0x1000;
    }

    assert(heap->start_address + new_size <= heap->max_address);

    u32 old_size = heap->end_address - heap->start_address;

    u32 it = old_size;
    while(it < new_size) {
        alloc_frame(get_page(heap->start_address+it, 1, kernel_directory),
        (heap->supervisor) ? 1 : 0, (heap->readonly) ? 0 : 1);
        it += 0x1000; // Page size
    }
    heap->end_address = heap->start_address+new_size;
}

static u32 contract(u32 new_size, heap_t *heap) {
    assert(new_size < heap->end_address - heap->start_address);

    if(new_size&0x1000) {
        new_size &= 0x1000;
        new_size += 0x1000;
    }

    if(new_size < HEAP_MIN_SIZE)
        new_size = HEAP_MIN_SIZE;
    
    u32 old_size = heap->end_address - heap->start_address;
    u32 it = old_size - 0x1000;
    while(new_size < it) {
        free_frame(get_page(heap->start_address+it, 0, kernel_directory));
        it -= 0x1000;
    }

    heap->end_address = heap->start_address + new_size;
    return new_size;
}

static u32 find_smallest_hole(u32 size, u8 page_align, heap_t *heap) {
    u32 it = 0;

    // Find smallest hole that fit our request
    while(it < heap->index.size) {
        header_t *head = (header_t*)lookup_ordered_array(it, &heap->index);
        if(page_align > 0) {
            // page must be aligned?
            u32 location = (u32)head;
            u32 offset = 0;
            if(((location+sizeof(header_t)) & 0xFFFFF000) != 0)
                offset = 0x1000 - (location+sizeof(header_t))%0x1000;
            u32 hole_size = (u32)head->size - offset;
            // Check if we can fit this page in that hole
            if(hole_size >= (u32)size)
                break;
        } else if(head->size >= size)
            break;
        it++;
    }

    // If we didn't find anything
    if(it == heap->index.size)
        return -1;
    else
        return it;
}

static u8 header_t_less_than(void *a, void *b) {
    return (((header_t*)a)->size < ((header_t*)b)->size)?1:0;
}

heap_t *create_heap(u32 start, u32 end_addr, u32 max, u8 supervisor, u8 readonly) {
    heap_t *heap = (heap_t*)kmalloc(sizeof(heap_t));

    assert(start % 0x1000 == 0);
    assert(end_addr % 0x1000 == 0);

    // Initialize the index
    heap->index = place_ordered_array((void*)start, HEAP_INDEX_SIZE, &header_t_less_than);
    // Shift start address to the right
    start += sizeof(type_t) * HEAP_INDEX_SIZE;

    // Check if start address is page-aligned
    if ((start & 0xFFFFF000) != 0) {
        start &= 0xFFFFF000;
        start += 0x1000;
    }
    // Store vars into heap
    heap->start_address = start;
    heap->end_address = end_addr;
    heap->max_address = max;
    heap->supervisor = supervisor;
    heap->readonly = readonly;

    header_t *hole = (header_t*)start;
    hole->size = end_addr - start;
    hole->magic = HEAP_MAGIC;
    hole->is_hole = 1;
    insert_ordered_array((void*)hole, &heap->index);

    return heap;
}

void *alloc(u32 size, u8 page_align, heap_t *heap) {
    u32 new_size = size + sizeof(header_t) + sizeof(footer_t);
    // Find smallest hole suitable
    u32 it = find_smallest_hole(new_size, page_align, heap);

    if((int32_t)it == -1) {
        u32 old_len = heap->end_address - heap->start_address;
        u32 old_end_addr = heap->end_address;

        // Allocate more space
        expand(old_len+new_size, heap);
        u32 new_len = heap->end_address - heap->start_address;

        it = 0;
        u32 idx = -1; u32 value = 0x0;
        while(it < heap->index.size) {
            u32 tmp = (u32)lookup_ordered_array(it, &heap->index);
            if(tmp > value) {
                value = tmp;
                idx = it;
            }
            it++;
        }

        // If no headers has been found, add a new one
        if((int32_t)idx == -1) {
            header_t *head = (header_t*)old_end_addr;
            head->magic = HEAP_MAGIC;
            head->size = new_len - old_len;
            head->is_hole = 1;
    
            footer_t *foot = (footer_t*)(old_end_addr + head->size - sizeof(footer_t));
            foot->magic = HEAP_MAGIC;
            foot->header = head;
            insert_ordered_array((void*)head, &heap->index);
        } else {
            header_t *head = lookup_ordered_array(idx, &heap->index);
            head->size += new_len - old_len;
            // Update the footer
            footer_t *foot = (footer_t*)((u32)head + head->size - sizeof(footer_t));
            foot->header = head;
            foot->magic = HEAP_MAGIC;
        }        
        // Now we have enough space, so recall this function again
        return alloc(size, page_align, heap);
    }

    header_t *origin_hole_head = (header_t*)lookup_ordered_array(it, &heap->index);
    u32 origin_hole_p = (u32)origin_hole_head;
    u32 origin_hole_s = origin_hole_head->size;
    // Check if we should split the hole into two parts
    if(origin_hole_s-new_size < sizeof(header_t)+sizeof(footer_t)) {
        // Increase the requested size to the size of the hole we found
        size += origin_hole_s-new_size;
        new_size = origin_hole_s;
    }
    
    // Check if we need to page-align data
    if(page_align && origin_hole_p&0xFFFFF000) {
        u32 new_loc = origin_hole_p + 0x1000 - (origin_hole_p&0xFFF) - sizeof(header_t);
        header_t *hole_header = (header_t*)origin_hole_p;
        hole_header->size = 0x1000 - (origin_hole_p&0xFFF) - sizeof(header_t);
        hole_header->magic = HEAP_MAGIC;
        hole_header->is_hole = 1;
        footer_t *hole_footer = (footer_t*)((u32)new_loc - sizeof(header_t));
        hole_footer->magic = HEAP_MAGIC;
        hole_footer->header = hole_header;
        origin_hole_p = new_loc;
        origin_hole_s = origin_hole_s - hole_header->size;
    } else
        remove_ordered_array(it, &heap->index); // Remove hole, since we don't need it anymore

    // Rewrite original header
    header_t *block_head = (header_t*)origin_hole_p;
    block_head->magic = HEAP_MAGIC;
    block_head->is_hole = 0;
    block_head->size = new_size;
    // and the footer
    footer_t *block_foot = (footer_t*)(origin_hole_p + sizeof(header_t) + size);
    block_foot->magic = HEAP_MAGIC;
    block_foot->header = block_head;

    // Check if we need to write a new hole after the allocated block
    if(origin_hole_s - new_size > 0) {
        header_t *hole_head = (header_t*)(origin_hole_p + sizeof(header_t) + size + sizeof(footer_t));
        hole_head->magic = HEAP_MAGIC;
        hole_head->is_hole = 1;
        hole_head->size = origin_hole_s - new_size;
        footer_t *hole_foot = (footer_t*)((u32)hole_head + origin_hole_s - new_size - sizeof(footer_t));
        if((u32)hole_foot < heap->end_address) {
            hole_foot->magic = HEAP_MAGIC;
            hole_foot->header = hole_head;
        }
        // Add new hole to the data structure
        insert_ordered_array((void*)hole_head, &heap->index);
    }

    // Return the block header
    return (void*)((u32)block_head+sizeof(header_t));
}

void free(void *p, heap_t *heap) {
    // Check null pointers
    if(p == 0)
        return;

    // Retrieve data
    header_t *head = (header_t*)((u32)p - sizeof(header_t));
    footer_t *foot = (footer_t*)((u32)head + head->size - sizeof(footer_t));

    assert(head->magic == HEAP_MAGIC);
    assert(foot->magic == HEAP_MAGIC);

    head->is_hole = 1; // Make this a hole
    int8_t add_to_free_hole = 1; // Add this header to free holes

    // Left unify
    footer_t *test_foot = (footer_t*)((u32)head - sizeof(footer_t));
    if(test_foot->magic == HEAP_MAGIC && test_foot->header->is_hole == 1 ) {
        u32 cache_s = head->size; // Store current size
        head = test_foot->header; // Rewrite header into new one
        foot->header = head; // Point footer to the new header
        head->size += cache_s; // Increase size
        add_to_free_hole = 0; // Header already in the structure.
    }

    // Right unify
    header_t *test_head = (header_t*)((u32)foot + sizeof(footer_t));
    if(test_head->magic == HEAP_MAGIC && test_head->is_hole) {
        head->size += test_head->size; // Increase size
        test_foot = (footer_t*)((u32)test_foot + test_head->size - sizeof(footer_t));
        foot = test_foot;
    // Find and remove this header from the structure
    u32 it = 0;
    while((it < heap->index.size) && (lookup_ordered_array(it, &heap->index) != (void*)test_head))
        it++;
    
    assert(it < heap->index.size);

    // Remove that item
    remove_ordered_array(it, &heap->index);
    }

    // If footer is located at the end, we can contract the heap
    if((u32)foot+sizeof(footer_t) == heap->end_address) {
        u32 old_len = heap->end_address-heap->start_address;
        u32 new_len = contract((u32)head - heap->start_address, heap);
        // Check dimensions after resizing
        if(head->size - (old_len-new_len) > 0) {
            // Dimensions is still a positive value, so we can resize
            head->size -= old_len-new_len;
            foot = (footer_t*)((u32)head + head->size - sizeof(footer_t));
            foot->magic = HEAP_MAGIC;
            foot->header = head;
        } else {
            // Remove block from the structure
            u32 it = 0;
            while((it < heap->index.size) && (lookup_ordered_array(it, &heap->index) != (void*)test_head))
                it++;
            // If we didn't find that block we haven't nothing to remove
            if(it < heap->index.size)
                remove_ordered_array(it, &heap->index);
        }
    }

    // If required by the user, add that block to the structure
    if(add_to_free_hole == 1)
        insert_ordered_array((void*)head, &heap->index);
}