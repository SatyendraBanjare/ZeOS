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

typedef struct heap_stats {
    uint32_t heap_start;       /* first heap address */
    uint32_t heap_end;         /* current end of the heap (brk) */
    uint32_t heap_limit;       /* highest address the heap may grow to */
    uint32_t used_bytes;       /* payload bytes in allocated blocks */
    uint32_t free_bytes;       /* payload bytes in free blocks (reusable) */
    uint32_t overhead_bytes;   /* block headers */
    uint32_t used_blocks;
    uint32_t free_blocks;
    uint32_t largest_free;     /* biggest reusable block */
} heap_stats_t;

void heap_get_stats(heap_stats_t *st);

#endif