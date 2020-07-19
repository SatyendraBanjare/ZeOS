#include "../include/cpu/isr.h"

isr_t interrupt_handlers[MAX_IDT_ENTRIES_COUNT];

void isr_install() {
    make_idt_entry(0, (uint32_t)isr0);
    make_idt_entry(1, (uint32_t)isr1);
    make_idt_entry(2, (uint32_t)isr2);
    make_idt_entry(3, (uint32_t)isr3);
    make_idt_entry(4, (uint32_t)isr4);
    make_idt_entry(5, (uint32_t)isr5);
    make_idt_entry(6, (uint32_t)isr6);
    make_idt_entry(7, (uint32_t)isr7);
    make_idt_entry(8, (uint32_t)isr8);
    make_idt_entry(9, (uint32_t)isr9);
    make_idt_entry(10, (uint32_t)isr10);
    make_idt_entry(11, (uint32_t)isr11);
    make_idt_entry(12, (uint32_t)isr12);
    make_idt_entry(13, (uint32_t)isr13);
    make_idt_entry(14, (uint32_t)isr14);
    make_idt_entry(15, (uint32_t)isr15);
    make_idt_entry(16, (uint32_t)isr16);
    make_idt_entry(17, (uint32_t)isr17);
    make_idt_entry(18, (uint32_t)isr18);
    make_idt_entry(19, (uint32_t)isr19);
    make_idt_entry(20, (uint32_t)isr20);
    make_idt_entry(21, (uint32_t)isr21);
    make_idt_entry(22, (uint32_t)isr22);
    make_idt_entry(23, (uint32_t)isr23);
    make_idt_entry(24, (uint32_t)isr24);
    make_idt_entry(25, (uint32_t)isr25);
    make_idt_entry(26, (uint32_t)isr26);
    make_idt_entry(27, (uint32_t)isr27);
    make_idt_entry(28, (uint32_t)isr28);
    make_idt_entry(29, (uint32_t)isr29);
    make_idt_entry(30, (uint32_t)isr30);
    make_idt_entry(31, (uint32_t)isr31);

    // Remap the PIC
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0); 

    // Install the IRQs
    make_idt_entry(32, (uint32_t)irq0);
    make_idt_entry(33, (uint32_t)irq1);
    make_idt_entry(34, (uint32_t)irq2);
    make_idt_entry(35, (uint32_t)irq3);
    make_idt_entry(36, (uint32_t)irq4);
    make_idt_entry(37, (uint32_t)irq5);
    make_idt_entry(38, (uint32_t)irq6);
    make_idt_entry(39, (uint32_t)irq7);
    make_idt_entry(40, (uint32_t)irq8);
    make_idt_entry(41, (uint32_t)irq9);
    make_idt_entry(42, (uint32_t)irq10);
    make_idt_entry(43, (uint32_t)irq11);
    make_idt_entry(44, (uint32_t)irq12);
    make_idt_entry(45, (uint32_t)irq13);
    make_idt_entry(46, (uint32_t)irq14);
    make_idt_entry(47, (uint32_t)irq15);

    init_idt();
}