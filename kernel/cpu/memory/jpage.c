// #include "../../include/cpu/memory/jpage.h"
// #include "../../include/multiboot/multiboot.h"
// #include "../../include/multiboot/multiboot_util.h"

// // Refer to this for better Understanding [ https://www.clear.rice.edu/comp425/slides/L31.pdf ]
// #define PAGE_DIRECTORY_OFFSET_BITS 10
// #define PAGE_TABLE_OFFSET_BITS 10
// #define PAGE_OFFSET_BITS 12

// #define KERNEL_PAGE_TABLE_NUMBER 768 	// This Corresponds to 0xC0000000 address value.
// 									 	// This is also the address where the kernel is mapped

// #define BITMAP_SIZE 4096 // this is equal to nearest 32 multiple ceil {num_free_pages/8}  

// uint32_t free_pages; // Total Number/Size of free pages {32463 or 126 mb}
// uint32_t free_page_bitmap[BITMAP_SIZE];

// enum page_size_t { FOUR_KB, // 0 -> 4kb , We will be using 4 Kb pages only.
// 				   FOUR_MB  // 1 -> 4mb
// 				  };
// enum page_privilege_t { SUPERVISOR, // 0
// 					    USER        // 1 maybe referred to as being access by the kernel/ kenel memory
// 					   };
// enum page_permissions_t { READ_ONLY, // 0 
// 						  READ_WRITE // 1 , always 1 will be chosen for now.
// 						};

// // Bitmap Operations Implementations
// // ---------------------------------------------------------------

// // Mark Free for use
// static void set_frame(uint32_t page_number){
//   uint32_t index = page_number >> 5;         // get index number , equiv to div by 32
//   uint32_t bit = page_number & 0b11111;      // get offset number

//   free_page_bitmap[index] |= (1 << bit);    // Update the bitmap
// }

// // Mark Unavailable
// static void clear_frame(uint32_t page_number){
//   uint32_t index = page_number >> 5;         // get index number , equiv to div by 32
//   uint32_t bit = page_number & 0b11111;      // get offset number

//   free_page_bitmap[index] &= ~(1 << bit);    // Update the bitmap
// }

// // -----------------------------------------------------------------


// static void mark_free(uint32_t page_number){
// 	set_frame(page_number);
// 	free_pages ++;
// }

// static void mark_unavailable(uint32_t page_number){
// 	clear_frame(page_number);
// 	free_pages--;
// }


// // This is to align address for paging
// static uint32_t round_up_to_nearest_page_start(uint32_t address) {
//   if ((address & 0xFFF) != 0) {
//     address &= 0xFFFFF000;
//     address += 0x00001000;
//   }
//   return address;
// }

// static uint32_t round_down_to_nearest_page_start(uint32_t address) {
//   if ((address & 0xFFF) != 0) {
//     address &= 0xFFFFF000;
//     address -= 0x00001000;
//   }
//   return address;
// }

// // Get the page frame
// struct page_table_entry_t* get_page(uint32_t address, struct page_directory_t* page_dir){} 

// // Add Page Frames
// struct page_table_entry_t* make_page_table_entry(){

// 	page_table_entry_t* temp_page_frame ;

// 	uint32_t present:1;
//     uint32_t rw:1;
//     uint32_t user:1;
//     uint32_t pwt:1;
//     uint32_t pcd:1;
//     uint32_t accessed:1;
//     uint32_t dirty:1;
//     uint32_t pat:1;
//     uint32_t global:1;
//     uint32_t unused:3;
//     uint32_t frame:20;



// }

// // Add Page Tables
// void make_page_directory_entry(){

// 	// make entry at 1023 as a pointer to itself.

// 	// make entry ar 768 equivalent to 3gb virtual mem start

// }

// struct page_directory_t initialize_directory(){}

// void initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo){
// 	  multiboot_memory_map_t * memory_map = (multiboot_memory_map_t *) p_to_v(mbinfo->mmap_addr);
// 	  uint32_t num_entries = mbinfo->mmap_length / sizeof(multiboot_memory_map_t);

// 	  print_log_int(num_entries,10); print_log("\n total enteries \n");

// 	  for (uint32_t i = 0; i < num_entries; i++) {

// 	  	// Make sure that memory is accessible and its address is not 0x000.
// 	  	// because the base address may be later used to compensate for the boot loader
// 	  	// and other boot related operations.
// 	    if ((memory_map[i].type == 1) && (memory_map[i].addr > 0)) {
	      
// 	      // Now we have memory loaction available
// 	      uint32_t start_mem_address = memory_map[i].addr; 				// get the start address
// 	      uint32_t end_mem_address = start_address + memory_map[i].len; // get the end address
	      
// 	      // Align the address to 4kb values  
// 	      uint32_t first_full_page = page_number(round_up_to_nearest_page_start(start_mem_address));
// 	      uint32_t one_past_last_full_page = page_number(round_down_to_nearest_page_start(end_mem_address));

// 	      // Now that we have perfectly aligned 4kb space, lets create empty page frames 
// 	      // int that space. Note that this is virtual space + physical space.
// 	      // and we will have to remove the physical paging later
// 	      for(uint32_t i = first_full_page; i < one_past_last_full_page; i++) {
// 	        mark_free(i);
// 	      }
// 	    } else {
// 	      print_log("NO free memory to start paging !!");
// 	      asm volatile("hlt");
// 	    }
// 	  }

// 	  // remove the paging in the physical space.
// 	  uint32_t first_partial_page = page_number(round_down_to_nearest_page_start(kernel_memory.kernel_physical_start));
// 	  uint32_t one_past_last_partial_page = page_number(round_up_to_nearest_page_start(kernel_memory.kernel_physical_end));

// 	  for(uint32_t i = first_partial_page; i < one_past_last_partial_page; i++) {
// 	    mark_unavailable(i);
// 	  }

// }