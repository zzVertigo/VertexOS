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

void isr_handler(registers_t regs) {
    u8 int_no = regs.int_no & 0xFF;

    if (interrupt_handlers[int_no] != NULL) {
        isr_t handler = interrupt_handlers[int_no];
        handler(&regs);
    } else {
		printf("Unhandled exception: [%d] -> %s", regs.int_no, exception_messages[regs.int_no]);
		
        for(;;);
    }
}

void irq_handler(registers_t regs) {
    if (regs.int_no >= 40)
        outportb(0xA0, 0x20);

    outportb(0x20, 0x20);

    if (interrupt_handlers[regs.int_no] != NULL) {
        isr_t handler = interrupt_handlers[regs.int_no];
        handler(&regs);
    }
}