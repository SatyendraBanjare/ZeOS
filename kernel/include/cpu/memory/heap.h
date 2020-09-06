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

void* zmalloc(uint32_t bytes);
void* zmalloc_p(uint32_t bytes, uint32_t* phys);

#endif