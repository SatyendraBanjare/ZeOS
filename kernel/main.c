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

void printuptime(int input)
{
    char buffer[11] = "0000000000";
    for (int i = 0; i < 10; i++)
    {
        int temp = input % 10;
        buffer[10 - i] = (char)(temp + 0x30);
        input /= 10;
    }
    zprint_time(buffer);
}

uint32_t trigger_page_fault() {
  uint32_t* unmapped_address = (uint32_t *) 0x500000; // 5 MB
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
	// isr_install();
	// irq_install();

	// Define Paging. 

	init_timer(50);


	init_shell();
	
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

	// Working Page Fault uncomment to see in action !!
	// uint32_t fault = trigger_page_fault();


	while(1){
		printuptime(rdtsc());
	}

}
