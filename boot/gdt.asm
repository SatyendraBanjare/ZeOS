[GLOBAL load_gdt]    ; Allows the C code to call load_gdt().

load_gdt:
	mov edx, [esp + 4]
	lgdt [edx]
	ret