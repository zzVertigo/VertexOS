#include <cpu/idt.h>
#include <cpu/irq.h>

extern u32 irq_vector[];
irq_handler_t irq_handlers[16];

void irq_setup() {
    for (u32 i = 0; i < 16; i++) {
        idt_entry(32 + i, irq_vector[i], 0x08, INTGATE);
    }
}

irq_handler_t irq_register(int index, irq_handler_t handler) {
    if (index < 16) {
        irq_handlers[index] = handler;
        return handler;
    }

    return NULL;
}

reg32_t irq_handler(reg32_t esp, processor_context_t context) {
    
    if (irq_handlers[context.int_no] != NULL) {
        esp = irq_handlers[context.int_no](esp, &context);
    } else {
        // unhandled irq
    }

    if (context.int_no >= 8) {
        out8(0xA0, 0x20);
    }

    out8(0x20, 0x20);
    
    return esp;
}
