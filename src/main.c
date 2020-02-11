#include <runtime.h>
#include <multiboot.h>

#include <system/chelpers.h>
#include <system/paging.h>
#include <system/task.h>
#include <system/heap.h>
#include <system/filesystem.h>
#include <system/ramdisk.h>

#include <system/devices/timer.h>
#include <system/devices/processor.h>
#include <system/devices/display.h>
#include <system/devices/cmos.h>

#include <system/display/textmode.h>

#include <cpu/descriptors.h>

static multiboot_info_t mbootinfo = {0};

extern u32 placement_address;

u32 initial_esp;

void kmain(multiboot_info_t *mboot, unsigned int magic, u32 initial_stack) {
    memcpy(&mbootinfo, mboot, sizeof(multiboot_info_t));

    initial_esp = initial_stack;

    // lol
    if (magic != MULTIBOOT_BOOTLOADER_MAGIC) {
        panic("Incorrect bootloader")
    }

    // basic vga drivers for displaying text
    textmode_setup();

    // gdt, idt, isr and irq
    descriptors_setup();

    // enable interrupts
    sti();

    timer_setup();

    // setup paging
    if (mboot->flags & MULTIBOOT_INFO_MEMORY) {
        paging_setup(mboot->mem_upper + mboot->mem_lower);
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

                    paging_mark_system((mmap->addr + i) & 0xFFFFF000);
                }
            }
            mmap = (multiboot_memory_map_t*)((uintptr_t)mmap + mmap->size + sizeof(uintptr_t));
        }
    } else {
        panic("Failed to configure memory map");
    }

    tasking_setup();

    u32 initrd_location = *((u32*)mboot->mods_addr);

    fs_root = init_fs(initrd_location);

    switch_to_user_mode();
}