#include "include/multiboot.h"
#include "include/cpu/gdt.h"
#include "include/cpu/idt.h"
#include "include/cpu/isr.h"
#include "include/terminal/terminal.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

void init_descriptor_tables(){
	
	//initialize GDT table
	init_gdt();

	//initialize IDT table
	init_idt();
}

void kernel_main(void) {

	init_descriptor_tables();

    terminal_initialize();
    terminal_writestring("Finally Booted successfully !!\n");
    terminal_writestring("Hello, world!!!\n");
}
