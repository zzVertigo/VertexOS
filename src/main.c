#include <runtime.h>
#include <multiboot.h>

#include <system/chelpers.h>
#include <system/filesystem.h>
#include <system/ramdisk.h>
#include <system/paging.h>
#include <system/thread.h>

#include <system/devices/timer.h>
#include <system/devices/processor.h>
#include <system/devices/display.h>
#include <system/devices/cmos.h>
#include <system/devices/keyboard.h>

#include <drivers/textmode.h>

#include <cpu/descriptors.h>

static multiboot_info_t mbootinfo = {0};

extern u32 placement_addr;

u32 initial_esp;

void kmain(multiboot_info_t *mboot, u32 magic, u32 initial_stack) {
    memcpy(&mbootinfo, mboot, sizeof(multiboot_info_t));

    initial_esp = initial_stack;

    // check to see if we are using the correct bootloader
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
       panic("Incorrect bootloader");
    }

    // basic vga drivers for displaying text
    textmode_setup();

    printf("[*] Bootloader Magic: 0x%x\n", magic);
    printf("[*] Initial ESP: 0x%x\n", initial_esp);
    printf("[*] Heap Placement: 0x%x\n\n", placement_addr);

    printf("[*] Beginning pre-boot setup\n\n");

    printf("[*] Descriptors Table... ");
    {
        // gdt, idt, isr and irq
        descriptors_setup();
        printf("done.\n");
    }

    printf("[*] Timer(s)... ");
    {
        timer_setup();
        printf("done.\n");
    }

    // setup paging
    if (mboot->flags & MULTIBOOT_INFO_MEMORY) {
        printf("[*] Paging... ");
        {
            paging_setup(mboot->mem_upper + mboot->mem_lower);

            printf("done.\n\n");

            paging_test();
        }
    } else {
        panic("Failed to configure memory paging");
    }

    threading_setup();
    {
        threading_test();
    }

    keyboard_setup();

    if (mboot->mods_count > 0) {
        u32 initrd_start = *((u32*)mboot->mods_addr);

        fs_root = init_fs(initrd_start);
    } else {
        printf("[*] No ramdisk mounted... continuing!\n\n");
    }

    // clear_screen();

    printf("%s [Version %d.%d.%d]\n", NAME, MAJOR, MINOR, BUILD);
    printf("(c) %d %s. All rights reserved.\n\n", COPYRIGHT_YEAR, COPYRIGHT_OWNER);

    printf("> ");

    while (1) {
        char c = get_ascii_char();

        printf("%c", c);
    }
}