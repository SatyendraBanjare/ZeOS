#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../cpu/util.h"
#include "../common/helper.h"
#include "../cpu/isr.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25

#define VIDEO_ADDRESS 0xb8000

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

#define CMD_COLOR       0x0f
#define CURSOR_COLOR    0x0a
#define HEADER_COLOR    0x4e
#define FOOTER_COLOR    0x01

/* Public kernel API */
void clear_screen();
void zprint_at(char *message, int col, int row);
void zprint(char *message);
void zprint_backspace();
void zprint_left();
void zprint_right();
void clear_screen_full();
int print_char_last(char c, int col, int row);
void zprint_at_last(char *message);

// void terminal_initialize(void);
// void terminal_writestring(const char* data);

#endif