#ifndef UTILH
#define UTILH

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/*
Very Basic Helper functions to perform read/write to resgisters.
*/
void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);

uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);

void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);

#endif
