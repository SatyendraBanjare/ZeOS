#ifndef JPAGE_H
#define JPAGE_H

#include <stdint.h>
#include <stddef.h>

#include "../../multiboot/multiboot.h"
#include "../../cpu/gdt.h"
#include "../../cpu/idt.h"
#include "../../cpu/isr.h"
#include "../../cpu/state.h"
#include "../../terminal/terminal.h"
#include "../../common/helper.h"
#include "../../multiboot/multiboot_util.h"

typedef struct page_table_entry
{
    uint32_t present:1;
    uint32_t rw:1;
    uint32_t user:1;
    uint32_t pwt:1;
    uint32_t pcd:1;
    uint32_t accessed:1;
    uint32_t dirty:1;
    uint32_t pat:1;
    uint32_t global:1;
    uint32_t unused:3;
    uint32_t frame:20;
}__attribute__((packed)) page_table_entry_t;
 
typedef struct page_table
{
    page_table_entry_t pages[1024]; 
}__attribute__((packed)) page_table_t;

typedef struct page_directory
{
	page_table_t * tables[1024];

	// Array to pointers of page tables.
    uint32_t tablesPhysical[1024]; 

    // Physical Address
    uint32_t physicalAddr
}__attribute__((packed)) page_directory_t;
 
void initialize_paging(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo);

struct page_table_entry_t* get_page(uint32_t address, struct page_directory_t* page_dir); 

#endif
