#include <runtime.h>
#include <multiboot.h>

#include <system/chelpers.h>
#include <system/filesystem.h>
#include <system/ramdisk.h>
#include <system/paging.h>
#include <system/thread.h>

#include <system/drivers/timer.h>
#include <system/drivers/processor.h>
#include <system/drivers/display.h>
#include <system/drivers/keyboard.h>

#include <system/drivers/textmode.h>

#include <arch/i386/cpu/descriptors.h>

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

    // mark system memory
    if (mboot->flags & MULTIBOOT_INFO_MEM_MAP) {
        multiboot_memory_map_t *mmap = (void*)mboot->mmap_addr;

        while ((uintptr_t)mmap < mboot->mmap_addr + mboot->mmap_length) {
            if (mmap->type == 2) {
                for (u32 i = 0; i < mmap->len; i += 0x1000) {
                    if (mmap->addr + i > 0xFFFFFFFF)
                        break;

                    mark_system_memory((mmap->addr + i) & 0xFFFFF000);
                }
            }
            mmap = (multiboot_memory_map_t*)((uintptr_t)mmap + mmap->size + sizeof(uintptr_t));
        }
    } else {
        panic("Failed to configure memory map");
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

    while (1) {
        char c = get_ascii_char();
        printf("%c", c);
    }
}