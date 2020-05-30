[org 0x7c00] ; Defines the entry point of our booloader

[bits 16] ; Enter the 16 bit Real Mode



times 510 - ($-$$) db 0 ; Padding to fit 512 byte size
dw 0xaa55 ; The boot record signaure