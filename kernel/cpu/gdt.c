#include "../include/cpu/gdt.h"

// Access methods defined in the asm files.
extern void load_gdt(uint32_t GDT_Ptr);

gdt_entry_t gdt_entries[GDT_ENTRIES_COUNT];
gdt_ptr_t gdt_ptr;

static void make_gdt_entry(uint32_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;

    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

void init_gdt()
{
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRIES_COUNT - 1 );
    gdt_ptr.base  = (uint32_t) &gdt_entries;

    make_gdt_entry(0, 0, 0, 0, 0);                // Null segment
    make_gdt_entry(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    make_gdt_entry(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    make_gdt_entry(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    make_gdt_entry(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment

    load_gdt((uint32_t)&gdt_ptr);
}

