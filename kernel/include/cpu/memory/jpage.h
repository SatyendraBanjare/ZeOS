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

typedef struct page
{
    u32int present    : 1;   // Page present in memory
    u32int rw         : 1;   // Read-only if clear, readwrite if set
    u32int user       : 1;   // Supervisor level only if clear
    u32int accessed   : 1;   // Has the page been accessed since last refresh?
    u32int dirty      : 1;   // Has the page been written to since last refresh?
    u32int unused     : 7;   // Amalgamation of unused and reserved bits
    u32int frame      : 20;  // Frame address (shifted right 12 bits)
}__attribute__((packed)) page_t;
 
typedef struct page_table
{
    page_t pages[1024]; 
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

page_t* get_page(uint32_t address, struct page_directory_t* page_dir);    

#endif
