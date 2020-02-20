#include <runtime.h>
#include <drivers/textmode.h>

void __panic(const char *file, u32 line, const char *func, const char *reason) {
    clear_screen();
    
    cli();

    printf("*** KERNEL PANIC ***\n\n");
    printf("Reason: %s\n", reason);
    printf("File: %s\n", file);
    printf("Line: %d\n", line);
    printf("Function: %s\n", func);

    // todo cpu dump

    hlt();
}

void __assert(const char *file, u32 line, const char* func, const char *reason) {
    clear_screen();
    
    cli();

    printf("\n\n*** ASSERTION FAILURE ***\n\n");
    printf("Reason: %s\n", reason);
    printf("File: %s\n", file);
    printf("Line: %d\n", line);
    printf("Function: %s\n", func);

    hlt();
}

void reboot() {
    asm volatile("cli");

    outportb(0x64, 0xFE); // reset cpu Xd
}