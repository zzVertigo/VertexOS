#pragma once

#include <runtime.h>

#include <system/processor.h>

#define DPL_KERNEL 0b00000000 // ring 0
#define DPL_USER 0b01100000   // ring 1

#define INTGATE 0x8E
#define TRAPGATE 0x8F
#define IDT_ENTRY_COUNT 256

typedef void (*int_handler_t)(processor_context_t *states);

typedef struct __packed 
{
    u16 size;
    u32 offset;
} idt_descriptor_t;

typedef struct __packed 
{
    u16 offset0_15;
    u16 selector;
    u8 zero;
    u8 type_attr;
    u16 offset16_31;
} idt_entry_t;

typedef struct __packed {
    idt_entry_t entries[IDT_ENTRY_COUNT];
    idt_descriptor_t descriptor;
    int_handler_t handlers[IDT_ENTRY_COUNT];
} idt_t;

void idt_setup();
void idt_entry(u8 index, u32 offset, u16 selector, u16 type);