#include <multiboot.h>

#include <system/cstring.h>
#include <system/processor.h>
#include <system/memory.h>
#include <system/display.h>

#include <cpu/gdt.h>
#include <cpu/idt.h>
#include <cpu/isr.h>
#include <cpu/irq.h>
#include <cpu/fpu.h>

extern u32 __end;

static multiboot_info_t mbootinfo = {0};

void kmain(multiboot_info_t *mboot, unsigned int magic) {
    memcpy(&mbootinfo, mboot, sizeof(multiboot_info_t));

    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        // panic - incorrect bootloader
    }

    textmode_setup();

    gdt_setup(); // global descriptor table
    idt_setup(); // interrupt descriptor table
    isr_setup(); // interrupt service routine
    irq_setup(); // interrupt requests

//    sti(); // enable interrupts

//    fpu_install(); // floating point unit
}