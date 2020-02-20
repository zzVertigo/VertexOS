#include <system/devices/processor.h>
#include <system/devices/display.h>

void processor_dump_context(registers_t context) {
    printf("EAX = 0x%x, EBX = 0x%x, ECX = 0x%x, EDX = 0x%x\n", context.eax, context.ebx, context.ecx, context.edx);
    printf("ESP = 0x%x, EIP = 0x%x, EBP = 0x%x, EDI = 0x%x\n", context.esp, context.eip, context.ebp, context.edi);
    printf("ESI = 0x%x, EFLAGS = 0x%x", context.esi, context.eflags);

}