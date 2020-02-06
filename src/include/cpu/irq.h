#pragma once

#include <runtime.h>
#include <system/processor.h>

typedef reg32_t (*irq_handler_t)(reg32_t, processor_context_t *);

void irq_setup();
irq_handler_t irq_register(int index, irq_handler_t handler);