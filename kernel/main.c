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
  uint32_t* unmapped_address = (uint32_t *) 0x00000000; 
  return *unmapped_address;
}

void kernel_main(struct kernel_memory_descriptor_t kernel_memory, struct  multiboot_info_t *mboot_ptr) {

	// Print Kernel mempry addresses
	print_log("kernel Memory Address : \n--------------- \n");
	print_log("kernel Phy end : 0x"); print_log_int(kernel_memory.kernel_physical_end,16);print_log("\n");
	print_log("kernel Phy start : 0x"); print_log_int(kernel_memory.kernel_physical_start,16);print_log("\n");
	print_log("kernel virt end : 0x"); print_log_int(kernel_memory.kernel_virtual_end,16);print_log("\n");
	print_log("kernel virt start : 0x"); print_log_int(kernel_memory.kernel_virtual_start,16);print_log("\n");
	print_log("\n\n");

	// Print the multiboot info
	print_log("Multiboot Info : \n--------------- \n");
	print_multiboot_info( mboot_ptr);
	print_log("\n\n");
	
	// init serial IO 
	serial_init();

	// Initialize the gdt & idt.
	init_gdt();
	init_idt();

	// init timer & pic
	init_timer(50);
	pic_init();
	
	// clear screen
	clear_screen_full();
	init_shell();
	
	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	// print_log("\nPaging \n \n");
	// uint32_t free_pages = initialize_page_allocator(kernel_memory, mboot_ptr);
	
	// print_log("Free Pages :"); print_log_int(free_pages,10); print_log(" or "); print_log_int(free_pages/256,10); print_log(" MB");  print_log("\n");

	

	// page_directory_t pd = initialize_page_directory();
	// print_log("\n \n Initialized Page Directory \n\n");
	// uint32_t pdnum = (uint32_t) pd;
	// print_log("Address of Page Directory : "); print_log_int(pdnum,16); print_log("\n");

	// print_page_directory(pd);

	// // init all different 
	
	// print_log("\n \nInitializing Descriptor Tables \n \n");

	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

	// Working Page Fault uncomment to see in action !!
	// uint32_t fault = trigger_page_fault();
	// print_log("INTERRUPT ISSUE ADDR:");
	// interrupt();
	// print_log_int(fault,16);
	while(1){}

}
