#pragma once

#include <runtime.h>

#define VMEM_START		0xC0000000
#define VMEM_INITIAL_SIZE	0x100000
#define VMEM_INDEX_SIZE		0x20000
#define VMEM_MAGIC		0x123890AB
#define VMEM_MIN_SIZE		0x70000

typedef struct ordered_array
{
	type_t* array;
	unsigned int size;
	unsigned int max_size;
	lessthan_predicate_t less_than;
} ordered_array_t;

struct vmem_header
{
	unsigned int magic;	/* Magic number, used for error checking and identification */
	unsigned char is_hole;	/* 1 if this is a hole, 0 if this is a block */
	unsigned int size;	/* Size of the block, including this and the footer */
};

struct vmem_footer
{
	unsigned int magic;		/* Magic number, same as in header */
	struct vmem_header* header;	/* Pointer to the block header */
};

typedef struct vmem_heap
{
	struct ordered_array index;	
	uintptr_t start_address;		/* The start of our allocated space */
	uintptr_t end_address;		/* The end of our allocated space.  May be expanded up to max_address */
	uintptr_t max_address;		/* The maximum address the heap can be expanded to */
	unsigned char supervisor;	/* Should extra pages requested by us be mapped as supervisor-only? */
	unsigned char readonly;		/* Should extra pages requested by us be mapped as read-only? */
} vmem_heap_t;