#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80;
#define VGA_HEIGHT 25;

/* Screen i/o ports */
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5


/* Public kernel API */
void clear_screen();
void kprint_at(char *message, int col, int row);
void kprint(char *message);
void kprint_backspace();
void kprint_left();
void kprint_right();
void clear_screen_full();
int print_char_last(char c, int col, int row);
void kprint_at_last(char *message);

// void terminal_initialize(void);
// void terminal_writestring(const char* data);

#endif