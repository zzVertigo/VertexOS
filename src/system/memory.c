#include <system/memory.h>
#include <system/cstring.h>

extern u32 __end;

uintptr_t kmem_addr = (uintptr_t)&__end;

size_t kmem_memtotal;
