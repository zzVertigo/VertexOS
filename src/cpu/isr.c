#include <cpu/isr.h>

isr_t interrupt_handlers[256];

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

void register_interrupt_handler(u8 n, isr_t handler) {
    interrupt_handlers[n] = handler;
}

// This should be a pointer to the processor context but it's not properly being passed...
void isr_handler(registers_t regs) {
    if (interrupt_handlers[regs.int_no] != 0) {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    } else {
		printf("Unhandled exception: [%d] -> %s\n\n", regs.int_no, exception_messages[regs.int_no]);
		
		//processor_dump_context(regs);

        hlt();
    }
}

void ack_irq(u32 int_num) {
	if (int_num >= 40)
        outportb(0xA0, 0x20);

    outportb(0x20, 0x20);
}

void irq_handler(registers_t regs) {
	ack_irq(regs.int_no);

    if (interrupt_handlers[regs.int_no] != 0) {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(regs);
    }
}