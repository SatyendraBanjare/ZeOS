#ifndef HEAP_H
#define HEAP_H

#include <stdint.h>
#include <stddef.h>

#include "../../cpu/gdt.h"
#include "../../cpu/idt.h"
#include "../../cpu/isr.h"
#include "../../cpu/state.h"
#include "../../terminal/terminal.h"
#include "../../common/helper.h"

void* get_current_stack_pointer(void);
void* malloc(uint32_t bytes);
void  free(void* ptr);
void* calloc(uint32_t count, uint32_t size);
void* realloc(void* ptr, uint32_t bytes);
uint32_t heap_used_bytes(void);

#endif