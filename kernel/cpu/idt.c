#include "../include/cpu/idt.h"

// Access methods defined in the asm files.
extern void load_idt(unit32_t IDT_Ptr);

idt_entry_t idt_entries[IDT_ENTRIES_COUNT];
idt_ptr_t idt_ptr;

static void idt_set_gate(u8int num, u32int base, u16int sel, u8int flags)
{
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt_entries[num].flags   = flags /* | 0x60 */;
}
