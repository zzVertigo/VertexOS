#pragma once

#include <runtime.h>
#include <system/processor.h>

typedef void (*isr_handler_t)(processor_context_t *);

void isr_setup();
isr_handler_t isr_register(int index, isr_handler_t handler);