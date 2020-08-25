#include "../../include/cpu/memory/jpage.h"
#include "../../include/multiboot/multiboot.h"
#include "../../include/multiboot/multiboot_util.h"

// Refer to this for better Understanding [ https://www.clear.rice.edu/comp425/slides/L31.pdf ]
#define PAGE_DIRECTORY_OFFSET_BITS 10
#define PAGE_TABLE_OFFSET_BITS 10
#define PAGE_OFFSET_BITS 12

#define KERNEL_PAGE_TABLE_NUMBER 768 	// This Corresponds to 0xC0000000 address value.
									 	// This is also the address where the kernel is mapped

#define BITMAP_SIZE 4096 // this is equal to nearest 32 multiple ceil {num_free_frames/8}  

uint32_t free_frames; // Total Number/Size of free pages {32463 or 126 mb}
uint32_t free_page_bitmap[BITMAP_SIZE];

enum page_size_t { FOUR_KB, // 0 -> 4kb , We will be using 4 Kb pages only.
				   FOUR_MB  // 1 -> 4mb
				  };
enum page_privilege_t { SUPERVISOR, // 0
					    USER        // 1 maybe referred to as being access by the kernel/ kenel memory
					   };
enum page_permissions_t { READ_ONLY, // 0 
						  READ_WRITE // 1 , always 1 will be chosen for now.
						};

// Bitmap Operations Implementations
// ---------------------------------------------------------------

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
// 	free_frames ++;
// }

// static void mark_unavailable(uint32_t page_number){
// 	clear_frame(page_number);
// 	free_frames--;
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



//--------------------------------------------------------------------------------------------------------------------------//

void memset(u8int *dest, u8int val, u32int len)
{
    u8int *temp = (u8int *)dest;
    for ( ; len != 0; len--) *temp++ = val;
}

//--------------------------------------------------------------------------------------------------------------------------//

page_directory_t *kernel_directory=0;

// The current page directory;
page_directory_t *current_directory=0;

// A bitset of frames - used or free.
uint32_t *frames;
uint32_t nframes;

// Defined in kheap.c
extern uint32_t placement_address;
extern heap_t *kheap;

// Macros used in the bitset algorithms.
#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

// Static function to set a bit in the frames bitset
static void set_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
static void clear_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
static uint32_t test_frame(uint32_t frame_addr)
{
    uint32_t frame = frame_addr/0x1000;
    uint32_t idx = INDEX_FROM_BIT(frame);
    uint32_t off = OFFSET_FROM_BIT(frame);
    return (frames[idx] & (0x1 << off));
}

// Static function to find the first free frame.
static uint32_t first_frame()
{
    uint32_t i, j;
    for (i = 0; i < INDEX_FROM_BIT(nframes); i++)
    {
        if (frames[i] != 0xFFFFFFFF) // nothing free, exit early.
        {
            // at least one bit is free here.
            for (j = 0; j < 32; j++)
            {
                uint32_t toTest = 0x1 << j;
                if ( !(frames[i]&toTest) )
                {
                    return i*4*8+j;
                }
            }
        }
    }
}

// Function to allocate a frame.
void alloc_frame(page_t *page, int is_kernel, int is_writeable)
{
    if (page->frame != 0)
    {
        return;
    }
    else
    {
        uint32_t idx = first_frame();
        if (idx == (uint32_t)-1)
        {
            // PANIC! no free frames!!
        }
        set_frame(idx*0x1000);
        page->present = 1;
        page->rw = (is_writeable)?1:0;
        page->user = (is_kernel)?0:1;
        page->frame = idx;
    }
}

// Function to deallocate a frame.
void free_frame(page_t *page)
{
    uint32_t frame;
    if (!(frame=page->frame))
    {
        return;
    }
    else
    {
        clear_frame(frame);
        page->frame = 0x0;
    }
}

void initialise_paging()
{
    // The size of physical memory. For the moment we 
    // assume it is 16MB big.
    // uint32_t mem_end_page = 0x1000000;
    
    // nframes = mem_end_page / 0x1000;
    frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(nframes));
    memset(frames, 0, INDEX_FROM_BIT(nframes));
    
    // Let's make a page directory.
    kernel_directory = (page_directory_t*)kmalloc_a(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;

    // Map some pages in the kernel heap area.
    // Here we call get_page but not alloc_frame. This causes page_table_t's 
    // to be created where necessary. We can't allocate frames yet because they
    // they need to be identity mapped first below, and yet we can't increase
    // placement_address between identity mapping and enabling the heap!
    int i = 0;
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
        get_page(i, 1, kernel_directory);

    // We need to identity map (phys addr = virt addr) from
    // 0x0 to the end of used memory, so we can access this
    // transparently, as if paging wasn't enabled.
    // NOTE that we use a while loop here deliberately.
    // inside the loop body we actually change placement_address
    // by calling kmalloc(). A while loop causes this to be
    // computed on-the-fly rather than once at the start.
    // Allocate a lil' bit extra so the kernel heap can be
    // initialised properly.
    i = 0;
    while (i < placement_address+0x1000)
    {
        // Kernel code is readable but not writeable from userspace.
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);
        i += 0x1000;
    }

    // Now allocate those pages we mapped earlier.
    for (i = KHEAP_START; i < KHEAP_START+KHEAP_INITIAL_SIZE; i += 0x1000)
        alloc_frame( get_page(i, 1, kernel_directory), 0, 0);

    // Before we enable paging, we must register our page fault handler.
    register_interrupt_handler(14, page_fault);

    // Now, enable paging!
    switch_page_directory(kernel_directory);

    // Initialise the kernel heap.
    kheap = create_heap(KHEAP_START, KHEAP_START+KHEAP_INITIAL_SIZE, 0xCFFFF000, 0, 0);
}


page_t *get_page(uint32_t address_idx, int make, page_directory_t *dir)
{
    // Turn the address into an index.
    // address /= 0x1000;
    // Find the page table containing this address.
    u32int table_idx = address_idx / 1024;

    if (dir->tables[table_idx]) // If this table is already assigned
    {
        return &dir->tables[table_idx]->pages[address_idx];
    }
    else if(make)
    {
        uint32_t tmp;
        dir->tables[table_idx] = (page_table_t*)zmalloc_p(sizeof(page_table_t), &tmp);
        memset(dir->tables[table_idx], 0, 0x1000);
        dir->tablesPhysical[table_idx] = tmp | 0x7; // PRESENT, RW, US.
        return &dir->tables[table_idx]->pages[address_idx];
    }
    else
    {
        return 0;
    }
}

void initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo){
	  multiboot_memory_map_t * memory_map = (multiboot_memory_map_t *) p_to_v(mbinfo->mmap_addr);
	  uint32_t num_entries = mbinfo->mmap_length / sizeof(multiboot_memory_map_t);

	  print_log_int(num_entries,10); print_log("\n total enteries \n");

	  for (uint32_t i = 0; i < num_entries; i++) {

	  	// Make sure that memory is accessible and its address is not 0x000.
	  	// because the base address may be later used to compensate for the boot loader
	  	// and other boot related operations.
	    if ((memory_map[i].type == 1) && (memory_map[i].addr > 0)) {
	      
	      // Now we have memory loaction available
	      uint32_t start_mem_address = memory_map[i].addr; 				// get the start address
	      uint32_t end_mem_address = start_address + memory_map[i].len; // get the end address
	      
	      // Align the address to 4kb values  
	      uint32_t first_full_page = page_number(round_up_to_nearest_page_start(start_mem_address));
	      uint32_t one_past_last_full_page = page_number(round_down_to_nearest_page_start(end_mem_address));

	      // Now that we have perfectly aligned 4kb space, lets create empty page frames 
	      // int that space. Note that this is virtual space + physical space.
	      // and we will have to remove the physical paging later
	      for(uint32_t i = first_full_page; i < one_past_last_full_page; i++) {
	        free_frames ++;
	      }
	    } else {
	      print_log("NO free memory to start paging !!");
	      asm volatile("hlt");
	    }
	  }

	  // remove the paging in the physical space.
	  uint32_t first_partial_page = page_number(round_down_to_nearest_page_start(kernel_memory.kernel_physical_start));
	  uint32_t one_past_last_partial_page = page_number(round_up_to_nearest_page_start(kernel_memory.kernel_physical_end));

	  for(uint32_t i = first_partial_page; i < one_past_last_partial_page; i++) {
	    free_frames --;
	  }


	  frames = (uint32_t*)kmalloc(INDEX_FROM_BIT(free_frames));

	  // Let's make a page directory.
    kernel_directory = (page_directory_t*)zmalloc(sizeof(page_directory_t));
    memset(kernel_directory, 0, sizeof(page_directory_t));
    current_directory = kernel_directory;






}