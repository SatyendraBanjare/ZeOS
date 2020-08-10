// #include "page_table.h"

#include "../../include/cpu/memory/paging.h"
#include "../../include/multiboot/multiboot.h"
// #include "../multiboot_utils.h"

#define PAGE_DIRECTORY_OFFSET_BITS 10
#define PAGE_TABLE_OFFSET_BITS 10
#define PAGE_OFFSET_BITS 12

#define PAGE_SIZE_BYTES 4096
#define PAGE_SIZE_DWORDS 1024

#define KERNEL_PAGE_TABLE_NUMBER 768

// #define BITMAP_SIZE 32768
// Enough room for 512 MB of RAM
// TODO: Find a more efficient way to initialize page allocator
#define BITMAP_SIZE 4096

uint32_t p_to_v(uint32_t physical_address) {
  return physical_address + (uint32_t) &KERNEL_VIRTUAL_BASE;
}


uint32_t free_pages;
uint32_t free_page_bitmap[BITMAP_SIZE];

void mark_free(uint32_t page_number);
void mark_unavailable(uint32_t page_number);
uint32_t page_number(uint32_t address);
enum page_size_t {FOUR_KB, FOUR_MB};
enum page_privilege_t {SUPERVISOR, USER};
enum page_permissions_t {READ_ONLY, READ_WRITE};

uint32_t make_page_directory_entry(
  void* page_table_physical_address,
  enum page_size_t page_size,
  bool cache_disabled,
  bool write_through,
  enum page_privilege_t privelage,
  enum page_permissions_t permissions,
  bool present
) {
  uint32_t entry = (uint32_t) page_table_physical_address;
  entry |= page_size << 7;
  entry |= cache_disabled << 4;
  entry |= write_through << 3;
  entry |= privelage << 2;
  entry |= permissions << 1;
  entry |= present;

  return entry;
}

uint32_t make_page_table_entry(
  void* page_frame_address,
  bool global,
  bool cache_disabled,
  bool write_through,
  enum page_privilege_t privelage,
  enum page_permissions_t permissions,
  bool present
) {
  uint32_t entry = (uint32_t) page_frame_address;
  entry |= global << 8;
  entry |= cache_disabled << 6;
  entry |= write_through << 3;
  entry |= privelage << 2;
  entry |= permissions << 1;
  entry |= present;

  return entry;
}

// 1 page = 1024 * 4 bytes = 4 kB
// 1 page table = 1024 pages = 4 mB
// 1 page directory = 1024 page tables = 4 gB

// Find a free physical page and return it's physical address
// This does NOT zero out the page
void* allocate_physical_page() {
  for (uint32_t index = 0; index < BITMAP_SIZE; index++) {
    if (free_page_bitmap[index] != 0) {
      // There is at least one free page in this chunk
      for (uint8_t bit = 0; bit < 32; bit++) {
        if ((free_page_bitmap[index] & (1 << bit)) != 0) {
          uint32_t page_number = index * 32 + bit;
          mark_unavailable(page_number);
          void* page_start = (void*) (page_number << PAGE_OFFSET_BITS);
          return  page_start;
        }
      }
    }
  }

  // Out of physical memory
  // TODO: Evict a page
  return 0;
}

// Assumes we are using recursive page tables.
// Last entry in page directory points to itself
void* page_table_virtual_address(uint16_t page_table_number) {
  // First 10 bits are set to 1
  uint32_t virtual_address = 0xFFC00000;

  // Next 10 bits index index into page directory
  virtual_address |= (page_table_number << PAGE_OFFSET_BITS);

  return (void*) virtual_address;
}

page_directory_t initialize_page_directory() {
  page_directory_t pd = (page_directory_t) &PageDirectoryVirtualAddress;

  // Make the last entry in the pd a pointer to itself
  uint32_t pde = make_page_directory_entry(
    (void*) &PageDirectoryPhysicalAddress,
    FOUR_KB,
    false,
    false,
    SUPERVISOR,
    READ_WRITE,
    true
  );
  pd[1023] = pde;

  // dedicate one page table for memory for the kernel
  void* page_table_physical_address = allocate_physical_page();
  pd[KERNEL_PAGE_TABLE_NUMBER] = make_page_directory_entry(
    page_table_physical_address,
    FOUR_KB,
    false,
    false,
    SUPERVISOR,
    READ_WRITE,
    true
  );

  // From here on out, we can access page tables with `page_table_virtual_address()`
  // Fill in the kernel page table entirely.
  page_table_t pt = (page_table_t) page_table_virtual_address(KERNEL_PAGE_TABLE_NUMBER);
  for (uint16_t i = 0; i < 1024; i++) {
    void* page_physical_address = allocate_physical_page();
    pt[i] = make_page_table_entry(
      page_physical_address,
      false,
      false,
      false,
      SUPERVISOR,
      READ_WRITE,
      true
    );
  }

  return pd;
}

void print_page_directory( page_directory_t pd) {
  for (int i = 0; i < 1024; i++) {
    uint32_t entry = pd[i];
    bool present = entry & 0x1;

    if (present) {
      print_log( "page directory entry ");
      char entry_no[10];
	  itoa(i,entry_no,16);
      // print_uint32( i);
      print_log(entry_no);
      print_log( " is present.\n");
    }
  }
}

uint32_t round_up_to_nearest_page_start(uint32_t address) {
  if ((address & 0xFFF) != 0) {
    address &= 0xFFFFF000;
    address += 0x00001000;
  }
  return address;
}

uint32_t round_down_to_nearest_page_start(uint32_t address) {
  if ((address & 0xFFF) != 0) {
    address &= 0xFFFFF000;
    address -= 0x00001000;
  }
  return address;
}

uint32_t page_number(uint32_t address) {
  return address >> PAGE_OFFSET_BITS;
}

void mark_free(uint32_t page_number) {
  uint32_t index = page_number >> 5;
  uint32_t bit = page_number & 0b11111;
  uint32_t value = free_page_bitmap[index];
  value |= (1 << bit);
  free_page_bitmap[index] = value;
  free_pages++;
}

void mark_unavailable(uint32_t page_number) {
  uint32_t index = page_number >> 5;
  uint32_t bit = page_number & 0b11111;
  uint32_t value = free_page_bitmap[index];
  value &= ~(1 << bit);
  free_page_bitmap[index] = value;
  free_pages--;
}

uint32_t initialize_page_allocator(struct kernel_memory_descriptor_t kernel_memory, multiboot_info_t* mbinfo) {
  multiboot_memory_map_t * memory_map = (multiboot_memory_map_t *) p_to_v(mbinfo->mmap_addr);
  uint32_t num_entries = mbinfo->mmap_length / sizeof(multiboot_memory_map_t);

  for (uint32_t i = 0; i < num_entries; i++) {
    if (memory_map[i].type == 1) {
      // Available
      uint32_t first_addr = memory_map[i].addr;
      uint32_t one_past_last_addr = first_addr + memory_map[i].len;
      uint32_t first_full_page = page_number(round_up_to_nearest_page_start(first_addr));
      uint32_t one_past_last_full_page = page_number(round_down_to_nearest_page_start(one_past_last_addr));

      for(uint32_t i = first_full_page; i < one_past_last_full_page; i++) {
        mark_free(i);
      }
    } else {
      // Unavailable
    }
  }

  uint32_t first_partial_page = page_number(round_down_to_nearest_page_start(kernel_memory.kernel_physical_start));
  uint32_t one_past_last_partial_page = page_number(round_up_to_nearest_page_start(kernel_memory.kernel_physical_end));

  for(uint32_t i = first_partial_page; i < one_past_last_partial_page; i++) {
    mark_unavailable(i);
  }

  // for(uint16_t i = 0; i < BITMAP_SIZE; i++) {
  //   print_uint16(LOG, i);
  //   print_uint32(LOG, free_page_bitmap[i]);
  //   log("\n");
  // }

  return free_pages;
}
