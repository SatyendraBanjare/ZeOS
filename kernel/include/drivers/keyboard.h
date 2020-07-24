#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stddef.h>

#include "../cpu/isr.h"
#include "../cpu/util.h"
#include "../common/helper.h"
#include "../terminal/terminal.h"

#include "../codes.h"
#include "../UserCredentials.h"

void init_keyboard();
int has_access();

#endif
