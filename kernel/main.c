#include "include/multiboot/multiboot.h"
#include "include/cpu/gdt.h"
#include "include/cpu/idt.h"
#include "include/cpu/isr.h"
#include "include/cpu/state.h"
#include "include/terminal/terminal.h"
#include "include/common/helper.h"
#include "include/multiboot/multiboot_util.h"
#include "include/cpu/memory/paging.h"

#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif


//check flag for if username & password are correct
int shell_access_flag = 0;

void init_descriptor_tables(){
	//initialize GDT table
	init_gdt();

	//initialize IDT table
	isr_install();
	// irq_install();
}

void kernel_main(struct kernel_memory_descriptor_t kernel_memory, uint32_t ebx) {

	//initialize isr &irq to help input the 
	// working outputting to serial output . outb(0x3F8,'A');

	multiboot_info_t *mboot_ptr = (multiboot_info_t *) ebx;

	print_multiboot_info( mboot_ptr);


	print_log("\n \n Paging \n \n");
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

	init_gdt();
	isr_install();
	// print_log("\n \nInitializing Descriptor Tables \n \n");
	// init_descriptor_tables();

	
	// print_log("Initializing Shell \n \n");
	// init_shell();

	





	
	// User Acces Stuff
	// while(shell_access_flag == 0){
	// 	if(has_access() == 1)
	// 	{
	// 		shell_access_flag = 1;
	// 		start_execution();
	// 	}
	// }

	// start_execution();


}
