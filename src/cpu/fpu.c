#include <cpu/fpu.h>

void fpu_install() {
    asm volatile("clts");

    size_t t;

    asm volatile("mov %%cr0, %0" : "=r"(t));

    t &= ~(1 << 2);
    t |= (1 << 1);

    asm volatile("mov %0, %%cr0" ::"r"(t));

    asm volatile("mov %%cr4, %0" : "=r"(t));
    t |= 3 << 9;
    asm volatile("mov %0, %%cr4" ::"r"(t));

    asm volatile("fninit");
}