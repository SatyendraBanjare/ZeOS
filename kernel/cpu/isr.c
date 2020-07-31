#include "../include/common/helper.h"
#include "../include/cpu/isr.h"
#include "../include/cpu/util.h"
#include "../include/cpu/timer.h"
#include "../include/terminal/terminal.h"

state_t interrupt_handlers[MAX_IDT_ENTRIES_COUNT];

void isr_install() {
    make_idt_entry(0, (uint32_t)isr0, 0x08, 0x8E);
    make_idt_entry(1, (uint32_t)isr1, 0x08, 0x8E);
    make_idt_entry(2, (uint32_t)isr2, 0x08, 0x8E);
    make_idt_entry(3, (uint32_t)isr3, 0x08, 0x8E);
    make_idt_entry(4, (uint32_t)isr4, 0x08, 0x8E);
    make_idt_entry(5, (uint32_t)isr5, 0x08, 0x8E);
    make_idt_entry(6, (uint32_t)isr6, 0x08, 0x8E);
    make_idt_entry(7, (uint32_t)isr7, 0x08, 0x8E);
    make_idt_entry(8, (uint32_t)isr8, 0x08, 0x8E);
    make_idt_entry(9, (uint32_t)isr9, 0x08, 0x8E);
    make_idt_entry(10, (uint32_t)isr10, 0x08, 0x8E);
    make_idt_entry(11, (uint32_t)isr11, 0x08, 0x8E);
    make_idt_entry(12, (uint32_t)isr12, 0x08, 0x8E);
    make_idt_entry(13, (uint32_t)isr13, 0x08, 0x8E);
    make_idt_entry(14, (uint32_t)isr14, 0x08, 0x8E);
    make_idt_entry(15, (uint32_t)isr15, 0x08, 0x8E);
    make_idt_entry(16, (uint32_t)isr16, 0x08, 0x8E);
    make_idt_entry(17, (uint32_t)isr17, 0x08, 0x8E);
    make_idt_entry(18, (uint32_t)isr18, 0x08, 0x8E);
    make_idt_entry(19, (uint32_t)isr19, 0x08, 0x8E);
    make_idt_entry(20, (uint32_t)isr20, 0x08, 0x8E);
    make_idt_entry(21, (uint32_t)isr21, 0x08, 0x8E);
    make_idt_entry(22, (uint32_t)isr22, 0x08, 0x8E);
    make_idt_entry(23, (uint32_t)isr23, 0x08, 0x8E);
    make_idt_entry(24, (uint32_t)isr24, 0x08, 0x8E);
    make_idt_entry(25, (uint32_t)isr25, 0x08, 0x8E);
    make_idt_entry(26, (uint32_t)isr26, 0x08, 0x8E);
    make_idt_entry(27, (uint32_t)isr27, 0x08, 0x8E);
    make_idt_entry(28, (uint32_t)isr28, 0x08, 0x8E);
    make_idt_entry(29, (uint32_t)isr29, 0x08, 0x8E);
    make_idt_entry(30, (uint32_t)isr30, 0x08, 0x8E);
    make_idt_entry(31, (uint32_t)isr31, 0x08, 0x8E);

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
    make_idt_entry(32, (uint32_t)irq0, 0x08, 0x8E);
    make_idt_entry(33, (uint32_t)irq1, 0x08, 0x8E);
    make_idt_entry(34, (uint32_t)irq2, 0x08, 0x8E);
    make_idt_entry(35, (uint32_t)irq3, 0x08, 0x8E);
    make_idt_entry(36, (uint32_t)irq4, 0x08, 0x8E);
    make_idt_entry(37, (uint32_t)irq5, 0x08, 0x8E);
    make_idt_entry(38, (uint32_t)irq6, 0x08, 0x8E);
    make_idt_entry(39, (uint32_t)irq7, 0x08, 0x8E);
    make_idt_entry(40, (uint32_t)irq8, 0x08, 0x8E);
    make_idt_entry(41, (uint32_t)irq9, 0x08, 0x8E);
    make_idt_entry(42, (uint32_t)irq10, 0x08, 0x8E);
    make_idt_entry(43, (uint32_t)irq11, 0x08, 0x8E);
    make_idt_entry(44, (uint32_t)irq12, 0x08, 0x8E);
    make_idt_entry(45, (uint32_t)irq13, 0x08, 0x8E);
    make_idt_entry(46, (uint32_t)irq14, 0x08, 0x8E);
    make_idt_entry(47, (uint32_t)irq15, 0x08, 0x8E);

    init_idt();
}

/* To print the message which defines every exception */
char *exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",

    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",

    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",

    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(cpu_state *state) {
    zprint("received interrupt: ");
    char s[3];
    int_to_ascii(state->int_no, s);
    zprint(s);
    zprint("\n");
    zprint(exception_messages[state->int_no]);
    zprint("\n");
}

void irq_handler(cpu_state *state) {
    /* After every interrupt we need to send an EOI to the PICs
     * or they will not send another interrupt again */
    if (state->int_no >= 40) outb(0xA0, 0x20); /* slave */
    outb(0x20, 0x20); /* master */

    /* Handle the interrupt in a more modular way */
    if (interrupt_handlers[state->int_no] != 0) {
        state_t handler = interrupt_handlers[state->int_no];
        handler(state);
    }
}

void register_interrupt_handler(uint8_t n, state_t handler) {
    interrupt_handlers[n] = handler;
}

void irq_install(){
	asm volatile("sti");
	init_timer(50);
    // init_keyboard();
}
