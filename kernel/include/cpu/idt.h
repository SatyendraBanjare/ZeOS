#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "isr.h"
#include "state.h"

#define MAX_IDT_ENTRIES_COUNT 256  // check here : https://wiki.osdev.org/Interrupt_Descriptor_Table

// Access methods defined in the asm files.
// extern void load_gdt();

// struct idt_entry_struct
// {
//     uint16_t base_lo;             // The lower 16 bits of the address to jump to when this interrupt fires.
//     uint16_t sel;                 // Kernel segment selector.
//     uint8_t  always0;             // This must always be zero.
//     uint8_t  flags;               // More flags. See documentation.
//     uint16_t base_hi;             // The upper 16 bits of the address to jump to.
// } __attribute__((packed));

// typedef struct idt_entry_struct idt_entry_t;

// struct idt_ptr_struct
// {
//     uint16_t limit;
//     uint32_t base;                // The address of the first element in our idt_entry_t array.
// } __attribute__((packed));

// typedef struct idt_ptr_struct idt_ptr_t;

// void make_idt_entry(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags);
void init_idt();

#endif