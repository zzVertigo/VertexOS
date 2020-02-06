MULTIBOOT_PAGE_ALIGN equ 1 << 0
MULTIBOOT_MEMORY_INFO equ 1 << 1
MULTIBOOT_VIDEO_MODE equ 1 << 2
MULTIBOOT_MAGIC equ 0x1BADB002

MULTIBOOT_FLAGS equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO
MULTIBOOT_CHECKSUM equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

    dd 0x00000000
	dd 0x00000000
	dd 0x00000000
	dd 0x00000000
	dd 0x00000000

	dd 0x00000000
	dd 800
	dd 600
	dd 32

section .bss
align 16

global __stack_bottom
global __stack_top

__stack_bottom:
resb 16384
__stack_top:

section .text
global _kstart:function (_kstart.end - _kstart)
_kstart:
        cli
        cld

        mov esp, __stack_top

        push eax
        push ebx

        extern kmain
        call kmain

        cli
.hang:  hlt
        jmp .hang
.end:
