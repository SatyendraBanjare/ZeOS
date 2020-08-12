#include "../include/common/helper.h"

#define SERIAL_OUTPUT 0x3F8

// Some helper functions
void int_to_ascii(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';

    reverse(str);
}

void hex_to_ascii(int n, char str[]) {
    append(str, '0');
    append(str, 'x');
    char zeros = 0;

    int32_t tmp;
    int i;
    for (i = 28; i > 0; i -= 4) {
        tmp = (n >> i) & 0xF;
        if (tmp == 0 && zeros == 0) continue;
        zeros = 1;
        if (tmp > 0xA) append(str, tmp - 0xA + 'a');
        else append(str, tmp + '0');
    }

    tmp = n & 0xF;
    if (tmp >= 0xA) append(str, tmp - 0xA + 'a');
    else append(str, tmp + '0');
}

/* K&R */
void reverse(char s[]) {
    int c, i, j;
    for (i = 0, j = strlen(s)-1; i < j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

/* K&R */
int strlen(char s[]) {
    int i = 0;
    while (s[i] != '\0') ++i;
    return i;
}

void append(char s[], char n) {
    int len = strlen(s);
    s[len] = n;
    s[len+1] = '\0';
}

void backspace(char s[]) {
    int len = strlen(s);
    s[len-1] = '\0';
}

/* K&R 
 * Returns <0 if s1<s2, 0 if s1==s2, >0 if s1>s2 */
int strcmp(char s1[], char s2[]) {
    int i;
    for (i = 0; s1[i] == s2[i]; i++) {
        if (s1[i] == '\0') return 0;
    }
    return s1[i] - s2[i];
}

int int_len(int x){
    int c=0;
    while(x){
        c = c+1;
        x = x/10;
    }
    return c;
}

char * int_to_str(int input){
    char *buffer = "";
    int ll = int_len( input);
    for (int i = 0; i < ll; i++)
    {
        int temp = input % 10;
        buffer[i] = (char)(temp + 0x30);
        input /= 10;
    }
    reverse(buffer);
    return buffer;
}

static void write_log_char(char c){
    outb(SERIAL_OUTPUT,c);
}

void print_log(char *message){
    int i =0;
    while (message[i] != 0) {
        write_log_char(message[i]);
        i++;
    }
}

char *itoa(int n, char *str, int base) {
    if (base < 2 || base > 16) {
        str[0] = '\0';
        return str;
    }
    char alph[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' }; // Alphabet
    int i, sign;
    if (base == 10 && (sign = n) < 0) n = -n; // Signed mode is useful just when working in base 10
    unsigned int m = (unsigned int)n; // Unsigned copy of n, to make calculations
    i = 0;
    do {
        str[i++] = alph[m % base];
    } while ((m /= base) > 0);
    if (base == 10 && sign < 0) str[i++] = '-'; // Signed mode is useful just when working in base 10
    str[i] = '\0';
    reverse(str);
    return str;
}

void print_log_int(int num, int base){
    char temp[10];
    itoa(num,temp,base);
    print_log(temp);
}

void zprint_int(int num){
    char temp[10];
    itoa(num,temp,10);
    zprint(temp);
}