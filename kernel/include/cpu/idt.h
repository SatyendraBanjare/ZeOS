#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "isr.h"
#include "state.h"

void init_idt();

#endif