#include "include/multiboot/multiboot.h"
#include "include/cpu/gdt.h"
#include "include/cpu/idt.h"
#include "include/cpu/isr.h"
#include "include/cpu/pic.h"
#include "include/cpu/timer.h"
#include "include/cpu/state.h"
#include "include/cpu/serial.h"
#include "include/terminal/terminal.h"
#include "include/common/helper.h"
#include "include/multiboot/multiboot_util.h"
#include "include/cpu/memory/paging.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

extern void enable_paging();

uint32_t trigger_page_fault() {
  uint32_t* unmapped_address = (uint32_t *) 0xC0100000; // 5 MB
  return *unmapped_address;
}

void kernel_main(struct kernel_memory_descriptor_t kernel_memory, struct  multiboot_info_t *mboot_ptr) {

	// Get the multiboot pointer
	// multiboot_info_t *mboot_ptr = (multiboot_info_t *) ebx;

	// Print the multiboot info
	

	clear_screen_full();

	// Initialize the gdt & idt.

	serial_init();

	print_multiboot_info( mboot_ptr);

	init_gdt();
	init_idt();

	init_timer(50);
	pic_init();
	
	init_shell();
	
	


	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	print_log("\nPaging \n \n");
	uint32_t free_pages = initialize_page_allocator(kernel_memory, mboot_ptr);
	
	print_log("Free Pages :"); print_log_int(free_pages,10); print_log(" or "); print_log_int(free_pages/256,10); print_log(" MB");  print_log("\n");


	page_directory_t pd = initialize_page_directory();
	print_log("\n \n Initialized Page Directory \n\n");
	uint32_t pdnum = (uint32_t) pd;
	print_log("Address of Page Directory : "); print_log_int(pdnum,16); print_log("\n");

	print_page_directory(pd);

	// // init all different 
	
	// print_log("\n \nInitializing Descriptor Tables \n \n");

	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	// Working Page Fault uncomment to see in action !!
	uint32_t fault = trigger_page_fault();
	print_log("FAULT ADDR:");
	print_log_int(fault,16);
	while(1){}

}
