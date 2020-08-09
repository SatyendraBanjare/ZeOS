[GLOBAL load_gdt]    ; Allows the C code to call load_gdt().

load_gdt:
    mov eax, [esp+4]  ; Get the pointer to the GDT, passed as a parameter.
    lgdt [eax]        ; Load the new GDT pointer
    ret