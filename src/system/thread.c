#include <system/thread.h>

volatile thread_t *current_thread;
volatile thread_t *ready_queue;

extern page_directory_t *kernel_directory;
extern page_directory_t *current_directory;

extern u32 initial_esp;
extern u32 read_eip();

u32 next_tid = 0;

void threading_setup() {
    cli();

    move_stack((void*)0xE0000000, 0x2000);

    current_thread = ready_queue = (thread_t*)kmalloc(sizeof(thread_t));
    current_thread->id = next_tid++;
    current_thread->esp = current_thread->ebp = 0;
    current_thread->eip = 0;
    current_thread->page_directory = current_directory;
    current_thread->next = 0;
    current_thread->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);

    sti();
}

void move_stack(void *new_stack_start, u32 size) {
    u32 i;

    for (i = (u32)new_stack_start;
        i >= ((u32)new_stack_start - size);
        i -= 0x1000) {
            alloc_frame(get_page(i, 1, current_directory), 0, 1);
        }

    u32 pd_addr;

    asm volatile("mov %%cr3, %0" : "=r" (pd_addr));
    asm volatile("mov %0, %%cr3" : : "r" (pd_addr));

    u32 old_stack_pointer; asm volatile("mov %%esp, %0" : "=r" (old_stack_pointer));
    u32 old_base_pointer;  asm volatile("mov %%ebp, %0" : "=r" (old_base_pointer));

    u32 offset = (u32)new_stack_start - initial_esp;

    u32 new_stack_pointer = old_stack_pointer + offset;
    u32 new_base_pointer  = old_base_pointer  + offset;

    memcpy((void*)new_stack_pointer, (void*)old_stack_pointer, initial_esp - old_stack_pointer);

    for(i = (u32)new_stack_start; i > (u32)new_stack_start-size; i -= 4)
    {
        u32 tmp = * (u32*)i;
        // If the value of tmp is inside the range of the old stack, assume it is a base pointer
        // and remap it. This will unfortunately remap ANY value in this range, whether they are
        // base pointers or not.
        if (( old_stack_pointer < tmp) && (tmp < initial_esp))
        {
        tmp = tmp + offset;
        u32 *tmp2 = (u32*)i;
        *tmp2 = tmp;
        }
    }

    asm volatile("mov %0, %%esp" : : "r" (new_stack_pointer));
    asm volatile("mov %0, %%ebp" : : "r" (new_base_pointer));
}

void thread_switch() {
    if (!current_thread)
        return;

    u32 esp, ebp, eip;

    asm volatile("mov %%esp, %0" : "=r"(esp));
    asm volatile("mov %%ebp, %0" : "=r"(ebp));

    eip = read_eip();

    // Have we just switched tasks?
    if (eip == 0x12345)
        return;

    // No, we didn't switch tasks. Let's save some register values and switch.
    current_thread->eip = eip;
    current_thread->esp = esp;
    current_thread->ebp = ebp;
    
    // Get the next task to run.
    current_thread = current_thread->next;
    // If we fell off the end of the linked list start again at the beginning.
    if (!current_thread) current_thread = ready_queue;

    eip = current_thread->eip;
    esp = current_thread->esp;
    ebp = current_thread->ebp;

    // Make sure the memory manager knows we've changed page directory.
    current_directory = current_thread->page_directory;

    // Change our kernel stack over.
    set_kernel_stack(current_thread->kernel_stack+KERNEL_STACK_SIZE);

    perform_thread_switch(eip, current_directory->physicalAddr, ebp, esp);
}

int fork() {
    cli();

    thread_t *parent_thread = (thread_t*)current_thread;

    page_directory_t *directory = clone_directory(current_directory);

    thread_t *new_thread = (thread_t*)kmalloc(sizeof(thread_t));
    new_thread->id = next_tid++;
    new_thread->esp = new_thread->ebp = 0;
    new_thread->eip = 0;
    new_thread->page_directory = directory;
    current_thread->kernel_stack = kmalloc_a(KERNEL_STACK_SIZE);
    new_thread->next = 0;

    thread_t *tmp_thread = (thread_t*)ready_queue;

    while (tmp_thread->next)
        tmp_thread = tmp_thread->next;

    tmp_thread->next = new_thread;

    u32 eip = read_eip();

    if (current_thread == parent_thread) {
        u32 esp; asm volatile("mov %%esp, %0" : "=r"(esp));
        u32 ebp; asm volatile("mov %%ebp, %0" : "=r"(ebp));

        new_thread->esp = esp;
        new_thread->ebp = ebp;
        new_thread->eip = eip;

        sti();

        return new_thread->id;
    } else
        return -1;
}

void threading_test() {
    int test_thread_id = fork();

    printf("[*] Testing fork() - ");

    if (test_thread_id != -1)
        printf("Success! (%d)\n\n", test_thread_id);
    else
        printf("Failed! (%d)\n\n", test_thread_id);
}

int gettid() {
    return current_thread->id;
}