#pragma once

#include <runtime.h>
#include <system/paging.h>
#include <system/chelpers.h>

#define KERNEL_STACK_SIZE 2048

typedef struct thread {
    u32 id;

    u32 esp, ebp, eip;

    page_directory_t *page_directory;
    u32 kernel_stack;
    struct thread *next;
} thread_t;

void threading_setup();
void threading_test();
void thread_switch();
void move_stack(void *new_stack_start, u32 size);
extern void perform_thread_switch(u32, u32, u32, u32);