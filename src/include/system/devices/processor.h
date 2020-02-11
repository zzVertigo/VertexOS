#pragma once

#include <runtime.h>

typedef u32 reg32_t;

typedef struct __packed 
{
    reg32_t gs, fs, es, ds;
    reg32_t edi, esi, ebp, USELESS, ebx, edx, ecx, eax;
    reg32_t int_no, errcode;
    reg32_t eip, cs, eflags;
} processor_context_t;

void processor_dump_context(processor_context_t *context);

static inline reg32_t CR0()
{
    u32 r;
    asm volatile("mov %%cr0, %0"
                 : "=r"(r));
    return r;
}

static inline reg32_t CR1()
{
    u32 r;
    asm volatile("mov %%cr1, %0"
                 : "=r"(r));
    return r;
}

static inline reg32_t CR2()
{
    u32 r;
    asm volatile("mov %%cr2, %0"
                 : "=r"(r));
    return r;
}

static inline reg32_t CR3()
{
    u32 r;
    asm volatile("mov %%cr3, %0"
                 : "=r"(r));
    return r;
}

static inline reg32_t CR4()
{
    u32 r;
    asm volatile("mov %%cr4, %0"
                 : "=r"(r));
    return r;
}

static inline void cli(void) { asm volatile("cli"); }
static inline void sti(void) { asm volatile("sti"); }
static inline void hlt(void) { asm volatile("hlt"); }

#define _inline inline __attribute__((always_inline))

static _inline unsigned short inports(unsigned short _port) {
	unsigned short rv;
	asm volatile ("inw %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

static _inline void outports(unsigned short _port, unsigned short _data) {
	asm volatile ("outw %1, %0" : : "dN" (_port), "a" (_data));
}

static _inline unsigned int inportl(unsigned short _port) {
	unsigned int rv;
	asm volatile ("inl %%dx, %%eax" : "=a" (rv) : "dN" (_port));
	return rv;
}

static _inline void outportl(unsigned short _port, unsigned int _data) {
	asm volatile ("outl %%eax, %%dx" : : "dN" (_port), "a" (_data));
}

static _inline unsigned char inportb(unsigned short _port) {
	unsigned char rv;
	asm volatile ("inb %1, %0" : "=a" (rv) : "dN" (_port));
	return rv;
}

static _inline void outportb(unsigned short _port, unsigned char _data) {
	asm volatile ("outb %1, %0" : : "dN" (_port), "a" (_data));
}


static _inline void inportsm(unsigned short port, unsigned char * data, unsigned long size) {
	asm volatile ("rep insw" : "+D" (data), "+c" (size) : "d" (port) : "memory");
}