#include "../include/cpu/idt.h"

// Access methods defined in the asm files.
extern void load_idt(uint32_t IDT_Ptr);

idt_entry_t idt_entries[MAX_IDT_ENTRIES_COUNT];
idt_ptr_t idt_ptr;

// This will be used by install_isr() in isr.c
void make_idt_entry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt_entries[num].flags   = flags /* | 0x60 */;
}

void init_idt(){
	idt_ptr.base = (uint32_t) &idt_entries;
	idt_ptr.limit = MAX_IDT_ENTRIES_COUNT*sizeof(idt_entry_t) -1;

	load_idt((uint32_t) &idt_ptr);
}