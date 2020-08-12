#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "state.h"

void init_timer(uint32_t freq);
void timer_callback();
// uint32_t rdtsc();

#endif
