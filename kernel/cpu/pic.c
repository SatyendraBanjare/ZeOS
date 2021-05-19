#include "../include/cpu/pic.h"


#define PIC1    0x20    /* IO base address for master PIC */
#define PIC2    0xA0    /* IO base address for slave PIC */
#define PIC1_COMMAND  PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND  PIC2
#define PIC2_DATA (PIC2+1)

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04		/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */
 
#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08		/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C		/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

#define PIC_EOI   0x20    /* End-of-interrupt command code */

void pic_init() {
  // outb(PIC1_DATA, 0b11111111); // enable all interrupts , uncomment to start only keyboard interrupts
  // outb(PIC2_DATA, 0b11111111); // Don't enable any interrupts on slave pic (irc 8-15)
  // outb(PIC1_DATA, 0b11111101);
  // enable_hardware_interrupts();
  // outb(0x20, 0x11);
  //   outb(0xA0, 0x11);
  //   outb(0x21, 0x20);
  //   outb(0xA1, 0x28);
  //   outb(0x21, 0x04);
  //   outb(0xA1, 0x02);
  //   outb(0x21, 0x01);
  //   outb(0xA1, 0x01);
  //   outb(0x21, 0x0);
  //   outb(0xA1, 0x0); 
  // uint8_t a1, a2;
  //   a1 = inb(PIC1_DATA);
  //   io_wait();
  //   a2 = inb(PIC2_DATA);
  //   io_wait();

  //   outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
  //   io_wait();
  //   outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
  //   io_wait();

  //   outb(PIC1_DATA, 0x20);
  //   io_wait();
  //   outb(PIC2_DATA, 0x28);
  //   io_wait();

  //   outb(PIC1_DATA, 4);
  //   io_wait();
  //   outb(PIC2_DATA, 2);
  //   io_wait();

  //   outb(PIC1_DATA, ICW4_8086);
  //   io_wait();
  //   outb(PIC2_DATA, ICW4_8086);
  //   io_wait();

  //   outb(PIC1_DATA, a1);
  //   io_wait();
  //   outb(PIC2_DATA, a2);
  //   io_wait();

  asm volatile("sti");
}

void pic_acknowledge()
{
  outb(PIC1_COMMAND, PIC_EOI);
}

static inline void io_wait(void)
{
    /* Port 0x80 is used for 'checkpoints' during POST. */
    /* The Linux kernel seems to think it is free for use :-/ */
    asm volatile ( "outb %%al, $0x80" : : "a"(0) );
    /* %%al instead of %0 makes no difference.  TODO: does the register need to be zeroed? */
}