#include "../include/cpu/gdt.h"

// Access methods defined in the asm files.
extern void load_gdt(void * GDT_Ptr);
// void lgdt(void * gdt);


// gdt_entry_t gdt_entries[GDT_ENTRIES_COUNT];
// gdt_ptr_t gdt_ptr;

// static void make_gdt_entry(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
// {
//     gdt_entries[num].base_low    = (base & 0xFFFF);
//     gdt_entries[num].base_middle = (base >> 16) & 0xFF;
//     gdt_entries[num].base_high   = (base >> 24) & 0xFF;

//     gdt_entries[num].limit_low   = (limit & 0xFFFF);
//     gdt_entries[num].granularity = (limit >> 16) & 0x0F;

//     gdt_entries[num].granularity |= gran & 0xF0;
//     gdt_entries[num].access      = access;
// }

// void init_gdt()
// {
//     gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES_COUNT - 1 );
//     gdt_ptr.base  = (uint32_t) &gdt_entries;

//     make_gdt_entry(0, 0, 0, 0, 0);                // Null segment
//     make_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
//     make_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
//     make_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
//     make_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

//     load_gdt((uint32_t)&gdt_ptr);
// }

struct segment_descriptor_t {
  uint16_t limit_0_15; // bits o-15 of limit
  uint16_t base_0_15;
  uint8_t base_16_23;
  uint8_t access_byte;
  uint8_t flags_and_limit_16_19;
  uint8_t base_24_31;
} __attribute__((packed));

const uint16_t NULL_SEGMENT_SELECTOR = 0x0;
const uint16_t KERNAL_CODE_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * 1;
const uint16_t KERNAL_DATA_SEGMENT_SELECTOR = sizeof(struct segment_descriptor_t) * 2;

struct segment_descriptor_t gdt[3];

enum segment_selector_t {
  NULL_DESCRIPTOR, // Not used but has to be here
  KERNAL_CODE_SEGMENT_INDEX, // Offset 0x8
  KERNAL_DATA_SEGMENT_INDEX // Offset 0x10
};

// a pointer to the global descriptor table
// passed by reference to the LGDT instruction
struct gdt_description_structure_t {
  uint16_t size; // in bytes
  uint32_t offset;
} __attribute__((packed)) gdt_description_structure;

void init_gdt() {
  gdt_description_structure.size = sizeof(gdt) - 1;
  gdt_description_structure.offset = (uint32_t) gdt;

  gdt[KERNAL_CODE_SEGMENT_INDEX].limit_0_15 = 0xFFFF;
  gdt[KERNAL_CODE_SEGMENT_INDEX].base_0_15 = 0x0000;
  gdt[KERNAL_CODE_SEGMENT_INDEX].base_16_23 = 0x00;
  gdt[KERNAL_CODE_SEGMENT_INDEX].access_byte = 0b10011010;
  gdt[KERNAL_CODE_SEGMENT_INDEX].flags_and_limit_16_19 = 0xCF;
  gdt[KERNAL_CODE_SEGMENT_INDEX].base_24_31 = 0x00;

  gdt[KERNAL_DATA_SEGMENT_INDEX].limit_0_15 = 0xFFFF;
  gdt[KERNAL_DATA_SEGMENT_INDEX].base_0_15 = 0x0000;
  gdt[KERNAL_DATA_SEGMENT_INDEX].base_16_23 = 0x00;
  gdt[KERNAL_DATA_SEGMENT_INDEX].access_byte = 0b10010010;
  gdt[KERNAL_DATA_SEGMENT_INDEX].flags_and_limit_16_19 = 0xCF;
  gdt[KERNAL_DATA_SEGMENT_INDEX].base_24_31 = 0x00;

  load_gdt(&gdt_description_structure);

  // Grub has already loaded the segment registers
  // with the correct values (0x8 for cs, 0x10 for the others)
}
