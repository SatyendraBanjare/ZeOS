#include "include/multiboot/multiboot.h"
#include "include/cpu/gdt.h"
#include "include/cpu/idt.h"
#include "include/cpu/isr.h"
#include "include/cpu/pic.h"
#include "include/cpu/state.h"
#include "include/terminal/terminal.h"
#include "include/common/helper.h"
#include "include/multiboot/multiboot_util.h"
#include "include/cpu/memory/paging.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

extern void enable_paging();

void kernel_main(struct kernel_memory_descriptor_t kernel_memory, struct  multiboot_info_t *mboot_ptr) {

	// Get the multiboot pointer
	// multiboot_info_t *mboot_ptr = (multiboot_info_t *) ebx;

	// Print the multiboot info
	print_multiboot_info( mboot_ptr);

	clear_screen_full();

	// Initialize the gdt & idt.
	init_gdt();
	init_idt();
	// isr_install();
	// irq_install();

	// Define Paging. 

	pic_init();

	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	print_log("\nPaging \n \n");
	uint32_t free_pages = initialize_page_allocator(kernel_memory, mboot_ptr);
	char no[10];
	itoa(free_pages,no,16);
	print_log("Free Pages :"); print_log(no); print_log("\n");


	page_directory_t pd = initialize_page_directory();
	print_log("\n \n Initialized Page Directory \n\n");
	uint32_t pdnum = (uint32_t) pd;
	char pdno[10];
	itoa(pdnum,pdno,16);
	print_log("Address of Page Directory : "); print_log(pdno); print_log("\n");

	print_page_directory(pd);

	// // init all different 
	
	// print_log("\n \nInitializing Descriptor Tables \n \n");

	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	// init_descriptor_tables();

	
	// access shell

	// print_log("Initializing Shell \n \n");
	// init_shell();

	// while(1){}

}
