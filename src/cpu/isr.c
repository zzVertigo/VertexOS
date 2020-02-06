#include <cpu/idt.h>
#include <cpu/isr.h>

static const char *exception_messages[32] = {
        "Division by zero",
		"Debug",
		"Non-maskable interrupt",
		"Breakpoint",
		"Detected overflow",
		"Out-of-bounds",
		"Invalid opcode",
		"No coprocessor",
		"Double fault",
		"Coprocessor segment overrun",
		"Bad TSS",
		"Segment not present",
		"Stack fault",
		"General protection fault",
		"Page fault",
		"Unknown interrupt",
		"Coprocessor fault",
		"Alignment check",
		"Machine check",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved",
		"Reserved"
};

extern u32 isr_vector[];
static isr_handler_t isr_handlers[32] = {0};

void isr_setup() {
    for (u32 i = 0; i < 32; i++) {
        idt_entry(i, isr_vector[i], 0x08, INTGATE);
    }

    // todo 128 syscall handler
}

isr_handler_t isr_register(int index, isr_handler_t handler) {
    if (index < 32) {
        isr_handlers[index] = handler;
        return handler;
    }

    return NULL;
}

void isr_handler(processor_context_t context) {
    if (isr_handlers[context.int_no] != NULL) {
        isr_handlers[context.int_no](&context);
    } else {
        // handle interrupt 128
    }

    out8(0x20, 0x20);
}