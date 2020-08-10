#ifndef PAGING_H
#define PAGING_H

#include "../../multiboot/multiboot.h"
#include "../../cpu/gdt.h"
#include "../../cpu/idt.h"
#include "../../cpu/isr.h"
#include "../../cpu/state.h"
#include "../../terminal/terminal.h"
#include "../../common/helper.h"
#include "../../multiboot/multiboot_util.h"

void* allocate_page();
uint32_t initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo);

typedef uint32_t * page_directory_t;
typedef uint32_t * page_table_t;

page_directory_t initialize_page_directory();
void print_page_directory( page_directory_t pd);

#endif
