#include "../include/cpu/timer.h"
#include "../include/cpu/isr.h"

uint32_t tick = 0;

static void timer_callback(cpu_state *state) {
    tick++;
    (void)(state);
}

void init_timer(uint32_t freq) {
    /* Install the function we just wrote */
    register_interrupt_handler(IRQ0, timer_callback);

    /* Get the PIT value: hardware clock at 1193180 Hz */
    uint32_t divisor = 1193180 / freq;
    uint8_t low  = (uint8_t)(divisor & 0xFF);
    uint8_t high = (uint8_t)( (divisor >> 8) & 0xFF);
    /* Send the command */
    outb(0x43, 0x36); /* Command port */
    outb(0x40, low);
    outb(0x40, high);
}

// Read Time Stamp function
uint32_t rdtsc(void)
{   uint32_t time_h, time_l;
    // uint32_t eax, edx;
    asm volatile("rdtsc\n\t": "=a" (time_l), "=d" (time_h));

    return time_h;

}
