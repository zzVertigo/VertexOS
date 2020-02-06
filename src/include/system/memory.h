#pragma once

#include <runtime.h>
#include <multiboot.h>

uintptr_t kmalloc(size_t size);

void kfree(uintptr_t address);

void memory_setup(struct multiboot_info_t *mboot);