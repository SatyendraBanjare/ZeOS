#ifndef HELPER_H
#define HELPER_H

#include <stdint.h>
#include <stddef.h>

#include "../cpu/util.h"


void int_to_ascii(int n, char str[]);
void hex_to_ascii(int n, char str[]);
void reverse(char s[]);
int strlen(char s[]);
void backspace(char s[]);
void append(char s[], char n);
int strcmp(char s1[], char s2[]);
char * int_to_str(int input);
void print_log(char *message);
// void memory_copy(uint8_t *source, uint8_t *dest, int nbytes);

#endif