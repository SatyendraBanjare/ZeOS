#include "../include/cpu/pic.h"


#define PIC1    0x20    /* IO base address for master PIC */
#define PIC2    0xA0    /* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA (PIC2+1)

#define PIC_EOI   0x20    /* End-of-interrupt command code */

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

void pic_init() {
  uint8_t a1, a2;

    a1 = inb(PIC1_DATA);
    io_wait();
    a2 = inb(PIC2_DATA);
    io_wait();

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();

    outb(PIC1_DATA, 0x20);
    io_wait();
    outb(PIC2_DATA, 0x28);
    io_wait();

    outb(PIC1_DATA, 4);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, a1);
    io_wait();
    outb(PIC2_DATA, a2);
    io_wait();

  outb(PIC1_DATA, 0b11111101); // enable all interrupts , uncomment to start only keyboard interrupts
  outb(PIC2_DATA, 0b11111111); // Don't enable any interrupts on slave pic (irc 8-15)
  // enable_hardware_interrupts();
  asm volatile("sti");
  outb(PIC1_DATA, 0b11111000); // Create a timer Interrupt
	outb(PIC2_DATA, 0b11101111);
}

void pic_acknowledge()
{
  outb(PIC1_COMMAND, PIC_EOI);
}

void io_wait(){
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}