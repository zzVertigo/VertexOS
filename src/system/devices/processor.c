#include <system/devices/processor.h>
#include <system/devices/display.h>

void processor_dump_context(processor_context_t *context) {
    kprintf("CS=%04x DS=%04x ES=%04x FS=%04x GS=%04x\n", context->cs, context->ds, context->es, context->fs, context->gs);
    kprintf("");
    kprintf("");
    kprintf("");
}