;; Defined in isr.c

[extern isr_handler]
[extern irq_handler]

;; Common ISR code
isr_common_stub:
    
    ;; 1. Save CPU state
	pusha ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax
	mov ax, ds ; Lower 16-bits of eax = ds.
	push eax ; save the data segment descriptor
	mov ax, 0x10  ; kernel data segment descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	push esp ; cpu_state *r
    
    ; 2. Call C handler
    cld ; C code following the sysV ABI requires DF to be clear on function entry
	call isr_handler
	
    ; 3. Restore state
	pop eax 
    pop eax
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	popa
	add esp, 8 ; Cleans up the pushed error code and pushed ISR number
	iret ; pops 5 things at once: CS, EIP, EFLAGS, SS, and ESP

; Common IRQ code. Identical to ISR code except for the 'call' 
; and the 'pop ebx'
irq_common_stub:
    pusha 
    mov ax, ds
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    push esp
    cld
    call irq_handler ; Different than the ISR code
    pop ebx  ; Different than the ISR code
    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx
    popa
    add esp, 8
    iret 
	
; We don't get information about which interrupt was caller
; when the handler is run, so we will need to have a different handler
; for every interrupt.
; Furthermore, some interrupts push an error code onto the stack but others
; don't, so we will push a dummy error code for those which don't, so that
; we have a consistent stack for all of them.

; First make the ISRs global
global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

; IRQs
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15


global isr48
global isr49
global isr50
global isr51
global isr52
global isr53
global isr54
global isr55
global isr56
global isr57
global isr58
global isr59
global isr60
global isr61
global isr62
global isr63
global isr64
global isr65
global isr66
global isr67
global isr68
global isr69
global isr70
global isr71
global isr72
global isr73
global isr74
global isr75
global isr76
global isr77
global isr78
global isr79
global isr80
global isr81
global isr82
global isr83
global isr84
global isr85
global isr86
global isr87
global isr88
global isr89
global isr90
global isr91
global isr92
global isr93
global isr94
global isr95
global isr96
global isr97
global isr98
global isr99
global isr100
global isr101
global isr102
global isr103
global isr104
global isr105
global isr106
global isr107
global isr108
global isr109
global isr110
global isr111
global isr112
global isr113
global isr114
global isr115
global isr116
global isr117
global isr118
global isr119
global isr120
global isr121
global isr122
global isr123
global isr124
global isr125
global isr126
global isr127
global isr128
global isr129
global isr130
global isr131
global isr132
global isr133
global isr134
global isr135
global isr136
global isr137
global isr138
global isr139
global isr140
global isr141
global isr142
global isr143
global isr144
global isr145
global isr146
global isr147
global isr148
global isr149
global isr150
global isr151
global isr152
global isr153
global isr154
global isr155
global isr156
global isr157
global isr158
global isr159
global isr160
global isr161
global isr162
global isr163
global isr164
global isr165
global isr166
global isr167
global isr168
global isr169
global isr170
global isr171
global isr172
global isr173
global isr174
global isr175
global isr176
global isr177
global isr178
global isr179
global isr180
global isr181
global isr182
global isr183
global isr184
global isr185
global isr186
global isr187
global isr188
global isr189
global isr190
global isr191
global isr192
global isr193
global isr194
global isr195
global isr196
global isr197
global isr198
global isr199
global isr200
global isr201
global isr202
global isr203
global isr204
global isr205
global isr206
global isr207
global isr208
global isr209
global isr210
global isr211
global isr212
global isr213
global isr214
global isr215
global isr216
global isr217
global isr218
global isr219
global isr220
global isr221
global isr222
global isr223
global isr224
global isr225
global isr226
global isr227
global isr228
global isr229
global isr230
global isr231
global isr232
global isr233
global isr234
global isr235
global isr236
global isr237
global isr238
global isr239
global isr240
global isr241
global isr242
global isr243
global isr244
global isr245
global isr246
global isr247
global isr248
global isr249
global isr250
global isr251
global isr252
global isr253
global isr254
global isr255

; 0: Divide By Zero Exception
isr0:
    push byte 0
    push byte 0
    jmp isr_common_stub

; 1: Debug Exception
isr1:
    push byte 0
    push byte 1
    jmp isr_common_stub

; 2: Non Maskable Interrupt Exception
isr2:
    push byte 0
    push byte 2
    jmp isr_common_stub

; 3: Int 3 Exception
isr3:
    push byte 0
    push byte 3
    jmp isr_common_stub

; 4: INTO Exception
isr4:
    push byte 0
    push byte 4
    jmp isr_common_stub

; 5: Out of Bounds Exception
isr5:
    push byte 0
    push byte 5
    jmp isr_common_stub

; 6: Invalid Opcode Exception
isr6:
    push byte 0
    push byte 6
    jmp isr_common_stub

; 7: Coprocessor Not Available Exception
isr7:
    push byte 0
    push byte 7
    jmp isr_common_stub

; 8: Double Fault Exception (With Error Code!)
isr8:
    push byte 8
    jmp isr_common_stub

; 9: Coprocessor Segment Overrun Exception
isr9:
    push byte 0
    push byte 9
    jmp isr_common_stub

; 10: Bad TSS Exception (With Error Code!)
isr10:
    push byte 10
    jmp isr_common_stub

; 11: Segment Not Present Exception (With Error Code!)
isr11:
    push byte 11
    jmp isr_common_stub

; 12: Stack Fault Exception (With Error Code!)
isr12:
    push byte 12
    jmp isr_common_stub

; 13: General Protection Fault Exception (With Error Code!)
isr13:
    push byte 13
    jmp isr_common_stub

; 14: Page Fault Exception (With Error Code!)
isr14:
    push byte 14
    jmp isr_common_stub

; 15: Reserved Exception
isr15:
    push byte 0
    push byte 15
    jmp isr_common_stub

; 16: Floating Point Exception
isr16:
    push byte 0
    push byte 16
    jmp isr_common_stub

; 17: Alignment Check Exception
isr17:
    push byte 0
    push byte 17
    jmp isr_common_stub

; 18: Machine Check Exception
isr18:
    push byte 0
    push byte 18
    jmp isr_common_stub

; 19: Reserved
isr19:
    push byte 0
    push byte 19
    jmp isr_common_stub

; 20: Reserved
isr20:
    push byte 0
    push byte 20
    jmp isr_common_stub

; 21: Reserved
isr21:
    push byte 0
    push byte 21
    jmp isr_common_stub

; 22: Reserved
isr22:
    push byte 0
    push byte 22
    jmp isr_common_stub

; 23: Reserved
isr23:
    push byte 0
    push byte 23
    jmp isr_common_stub

; 24: Reserved
isr24:
    push byte 0
    push byte 24
    jmp isr_common_stub

; 25: Reserved
isr25:
    push byte 0
    push byte 25
    jmp isr_common_stub

; 26: Reserved
isr26:
    push byte 0
    push byte 26
    jmp isr_common_stub

; 27: Reserved
isr27:
    push byte 0
    push byte 27
    jmp isr_common_stub

; 28: Reserved
isr28:
    push byte 0
    push byte 28
    jmp isr_common_stub

; 29: Reserved
isr29:
    push byte 0
    push byte 29
    jmp isr_common_stub

; 30: Reserved
isr30:
    push byte 0
    push byte 30
    jmp isr_common_stub

; 31: Reserved
isr31:
    push byte 0
    push byte 31
    jmp isr_common_stub

; IRQ handlers
irq0:
	push byte 0
	push byte 32
	jmp irq_common_stub

irq1:
	push byte 1
	push byte 33
	jmp irq_common_stub

irq2:
	push byte 2
	push byte 34
	jmp irq_common_stub

irq3:
	push byte 3
	push byte 35
	jmp irq_common_stub

irq4:
	push byte 4
	push byte 36
	jmp irq_common_stub

irq5:
	push byte 5
	push byte 37
	jmp irq_common_stub

irq6:
	push byte 6
	push byte 38
	jmp irq_common_stub

irq7:
	push byte 7
	push byte 39
	jmp irq_common_stub

irq8:
	push byte 8
	push byte 40
	jmp irq_common_stub

irq9:
	push byte 9
	push byte 41
	jmp irq_common_stub

irq10:
	push byte 10
	push byte 42
	jmp irq_common_stub

irq11:
	push byte 11
	push byte 43
	jmp irq_common_stub

irq12:
	push byte 12
	push byte 44
	jmp irq_common_stub

irq13:
	push byte 13
	push byte 45
	jmp irq_common_stub

irq14:
	push byte 14
	push byte 46
	jmp irq_common_stub

irq15:
	push byte 15
	push byte 47
	jmp irq_common_stub


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

isr48:
    push byte 0 
    push byte 48
    jmp isr_common_stub 

isr49:
    push byte 0 
    push byte 49
    jmp isr_common_stub 

isr50:
    push byte 0 
    push byte 50
    jmp isr_common_stub 

isr51:
    push byte 0 
    push byte 51
    jmp isr_common_stub 

isr52:
      push byte 0 
      push byte 52
    jmp isr_common_stub 

isr53:
    push byte 0 
    push byte 53
    jmp isr_common_stub 

isr54:
    push byte 0 
    push byte 54
    jmp isr_common_stub 

isr55:
    push byte 0 
    push byte 55
    jmp isr_common_stub 

isr56:
    push byte 0 
    push byte 56
    jmp isr_common_stub 

isr57:
    push byte 0 
    push byte 57
    jmp isr_common_stub 

isr58:
    push byte 0 
    push byte 58
    jmp isr_common_stub 

isr59:
    push byte 0 
    push byte 59
    jmp isr_common_stub 

isr60:
    push byte 0 
    push byte 60
    jmp isr_common_stub 

isr61:
    push byte 0 
    push byte 61
    jmp isr_common_stub 

isr62:
    push byte 0 
    push byte 62
    jmp isr_common_stub 

isr63:
    push byte 0 
    push byte 63
    jmp isr_common_stub 

isr64:
    push byte 0 
    push byte 64
    jmp isr_common_stub 

isr65:
    push byte 0 
    push byte 65
    jmp isr_common_stub 

isr66:
    push byte 0 
    push byte 66
    jmp isr_common_stub 

isr67:
    push byte 0 
    push byte 67
    jmp isr_common_stub 

isr68:
    push byte 0 
    push byte 68
    jmp isr_common_stub 

isr69:
    push byte 0 
    push byte 69
    jmp isr_common_stub 

isr70:
    push byte 0 
    push byte 70
    jmp isr_common_stub 

isr71:
    push byte 0 
    push byte 71
    jmp isr_common_stub 

isr72:
    push byte 0 
    push byte 72
    jmp isr_common_stub 

isr73:
    push byte 0 
    push byte 73
    jmp isr_common_stub 

isr74:
    push byte 0 
    push byte 74
    jmp isr_common_stub 

isr75:
    push byte 0 
    push byte 75
    jmp isr_common_stub 

isr76:
    push byte 0 
    push byte 76
    jmp isr_common_stub 

isr77:
    push byte 0 
    push byte 77
    jmp isr_common_stub 

isr78:
    push byte 0 
    push byte 78
    jmp isr_common_stub 

isr79:
    push byte 0 
    push byte 79
    jmp isr_common_stub 

isr80:
    push byte 0 
    push byte 80
    jmp isr_common_stub 

isr81:
    push byte 0 
    push byte 81
    jmp isr_common_stub 

isr82:
    push byte 0 
    push byte 82
    jmp isr_common_stub 

isr83:
    push byte 0 
    push byte 83
    jmp isr_common_stub 

isr84:
    push byte 0 
    push byte 84
    jmp isr_common_stub 

isr85:
    push byte 0 
    push byte 85
    jmp isr_common_stub 

isr86:
    push byte 0 
    push byte 86
    jmp isr_common_stub 

isr87:
    push byte 0 
    push byte 87
    jmp isr_common_stub 

isr88:
    push byte 0 
    push byte 88
    jmp isr_common_stub 

isr89:
    push byte 0 
    push byte 89
    jmp isr_common_stub 

isr90:
    push byte 0 
    push byte 90
    jmp isr_common_stub 

isr91:
    push byte 0 
    push byte 91
    jmp isr_common_stub 

isr92:
    push byte 0 
    push byte 92
    jmp isr_common_stub 

isr93:
    push byte 0 
    push byte 93
    jmp isr_common_stub 

isr94:
    push byte 0 
    push byte 94
    jmp isr_common_stub 

isr95:
    push byte 0 
    push byte 95
    jmp isr_common_stub 

isr96:
    push byte 0 
    push byte 96
    jmp isr_common_stub 

isr97:
    push byte 0 
    push byte 97
    jmp isr_common_stub 

isr98:
    push byte 0 
    push byte 98
    jmp isr_common_stub 

isr99:
    push byte 0 
    push byte 99
    jmp isr_common_stub 

isr100:
    push byte 0 
    push byte 100
    jmp isr_common_stub 

isr101:
    push byte 0 
    push byte 101
    jmp isr_common_stub 

isr102:
    push byte 0 
    push byte 102
    jmp isr_common_stub 

isr103:
    push byte 0 
    push byte 103
    jmp isr_common_stub 

isr104:
    push byte 0 
    push byte 104
    jmp isr_common_stub 

isr105:
    push byte 0 
    push byte 105
    jmp isr_common_stub 

isr106:
    push byte 0 
    push byte 106
    jmp isr_common_stub 

isr107:
    push byte 0 
    push byte 107
    jmp isr_common_stub 

isr108:
    push byte 0 
    push byte 108
    jmp isr_common_stub 

isr109:
    push byte 0 
    push byte 109
    jmp isr_common_stub 

isr110:
    push byte 0 
    push byte 110
    jmp isr_common_stub 

isr111:
    push byte 0 
    push byte 111
    jmp isr_common_stub 

isr112:
    push byte 0 
    push byte 112
    jmp isr_common_stub 

isr113:
    push byte 0 
    push byte 113
    jmp isr_common_stub 

isr114:
    push byte 0 
    push byte 114
    jmp isr_common_stub 

isr115:
    push byte 0 
    push byte 115
    jmp isr_common_stub 

isr116:
    push byte 0 
    push byte 116
    jmp isr_common_stub 

isr117:
    push byte 0 
    push byte 117
    jmp isr_common_stub 

isr118:
    push byte 0 
    push byte 118
    jmp isr_common_stub 

isr119:
    push byte 0 
    push byte 119
    jmp isr_common_stub 

isr120:
    push byte 0 
    push byte 120
    jmp isr_common_stub 

isr121:
    push byte 0 
    push byte 121
    jmp isr_common_stub 

isr122:
    push byte 0 
    push byte 122
    jmp isr_common_stub 

isr123:
    push byte 0 
    push byte 123
    jmp isr_common_stub 

isr124:
    push byte 0 
    push byte 124
    jmp isr_common_stub 

isr125:
    push byte 0 
    push byte 125
    jmp isr_common_stub 

isr126:
    push byte 0 
    push byte 126
    jmp isr_common_stub 

isr127:
    push byte 0 
    push byte 127
    jmp isr_common_stub 

isr128:
    push byte 0 
    push byte 128
    jmp isr_common_stub 

isr129:
    push byte 0 
    push byte 129
    jmp isr_common_stub 

isr130:
    push byte 0 
    push byte 130
    jmp isr_common_stub 

isr131:
    push byte 0 
    push byte 131
    jmp isr_common_stub 

isr132:
    push byte 0 
    push byte 132
    jmp isr_common_stub 

isr133:
    push byte 0 
    push byte 133
    jmp isr_common_stub 

isr134:
    push byte 0 
    push byte 134
    jmp isr_common_stub 

isr135:
    push byte 0 
    push byte 135
    jmp isr_common_stub 

isr136:
    push byte 0 
    push byte 136
    jmp isr_common_stub 

isr137:
    push byte 0 
    push byte 137
    jmp isr_common_stub 

isr138:
    push byte 0 
    push byte 138
    jmp isr_common_stub 

isr139:
    push byte 0 
    push byte 139
    jmp isr_common_stub 

isr140:
    push byte 0 
    push byte 140
    jmp isr_common_stub 

isr141:
    push byte 0 
    push byte 141
    jmp isr_common_stub 

isr142:
    push byte 0 
    push byte 142
    jmp isr_common_stub 

isr143:
    push byte 0 
    push byte 143
    jmp isr_common_stub 

isr144:
    push byte 0 
    push byte 144
    jmp isr_common_stub 

isr145:
    push byte 0 
    push byte 145
    jmp isr_common_stub 

isr146:
    push byte 0 
    push byte 146
    jmp isr_common_stub 

isr147:
    push byte 0 
    push byte 147
    jmp isr_common_stub 

isr148:
    push byte 0 
    push byte 148
    jmp isr_common_stub 

isr149:
    push byte 0 
    push byte 149
    jmp isr_common_stub 

isr150:
    push byte 0 
    push byte 150
    jmp isr_common_stub 

isr151:
    push byte 0 
    push byte 151
    jmp isr_common_stub 

isr152:
    push byte 0 
    push byte 152
    jmp isr_common_stub 

isr153:
    push byte 0 
    push byte 153
    jmp isr_common_stub 

isr154:
    push byte 0 
    push byte 154
    jmp isr_common_stub 

isr155:
    push byte 0 
    push byte 155
    jmp isr_common_stub 

isr156:
    push byte 0 
    push byte 156
    jmp isr_common_stub 

isr157:
    push byte 0 
    push byte 157
    jmp isr_common_stub 

isr158:
    push byte 0 
    push byte 158
    jmp isr_common_stub 

isr159:
    push byte 0 
    push byte 159
    jmp isr_common_stub 

isr160:
    push byte 0 
    push byte 160
    jmp isr_common_stub 

isr161:
    push byte 0 
    push byte 161
    jmp isr_common_stub 

isr162:
    push byte 0 
    push byte 162
    jmp isr_common_stub 

isr163:
    push byte 0 
    push byte 163
    jmp isr_common_stub 

isr164:
    push byte 0 
    push byte 164
    jmp isr_common_stub 

isr165:
    push byte 0 
    push byte 165
    jmp isr_common_stub 

isr166:
    push byte 0 
    push byte 166
    jmp isr_common_stub 

isr167:
    push byte 0 
    push byte 167
    jmp isr_common_stub 

isr168:
    push byte 0 
    push byte 168
    jmp isr_common_stub 

isr169:
    push byte 0 
    push byte 169
    jmp isr_common_stub 

isr170:
    push byte 0 
    push byte 170
    jmp isr_common_stub 

isr171:
    push byte 0 
    push byte 171
    jmp isr_common_stub 

isr172:
    push byte 0 
    push byte 172
    jmp isr_common_stub 

isr173:
    push byte 0 
    push byte 173
    jmp isr_common_stub 

isr174:
    push byte 0 
    push byte 174
    jmp isr_common_stub 

isr175:
    push byte 0 
    push byte 175
    jmp isr_common_stub 

isr176:
    push byte 0 
    push byte 176
    jmp isr_common_stub 

isr177:
    push byte 0 
    push byte 177
    jmp isr_common_stub 

isr178:
    push byte 0 
    push byte 178
    jmp isr_common_stub 

isr179:
    push byte 0 
    push byte 179
    jmp isr_common_stub 

isr180:
    push byte 0 
    push byte 180
    jmp isr_common_stub 

isr181:
    push byte 0 
    push byte 181
    jmp isr_common_stub 

isr182:
    push byte 0 
    push byte 182
    jmp isr_common_stub 

isr183:
    push byte 0 
    push byte 183
    jmp isr_common_stub 

isr184:
    push byte 0 
    push byte 184
    jmp isr_common_stub 

isr185:
    push byte 0 
    push byte 185
    jmp isr_common_stub 

isr186:
    push byte 0 
    push byte 186
    jmp isr_common_stub 

isr187:
    push byte 0 
    push byte 187
    jmp isr_common_stub 

isr188:
    push byte 0 
    push byte 188
    jmp isr_common_stub 

isr189:
    push byte 0 
    push byte 189
    jmp isr_common_stub 

isr190:
    push byte 0 
    push byte 190
    jmp isr_common_stub 

isr191:
    push byte 0 
    push byte 191
    jmp isr_common_stub 

isr192:
    push byte 0 
    push byte 192
    jmp isr_common_stub 

isr193:
    push byte 0 
    push byte 193
    jmp isr_common_stub 

isr194:
    push byte 0 
    push byte 194
    jmp isr_common_stub 

isr195:
    push byte 0 
    push byte 195
    jmp isr_common_stub 

isr196:
    push byte 0 
    push byte 196
    jmp isr_common_stub 

isr197:
    push byte 0 
    push byte 197
    jmp isr_common_stub 

isr198:
    push byte 0 
    push byte 198
    jmp isr_common_stub 

isr199:
    push byte 0 
    push byte 199
    jmp isr_common_stub 

isr200:
    push byte 0 
    push byte 200
    jmp isr_common_stub 

isr201:
    push byte 0 
    push byte 201
    jmp isr_common_stub 

isr202:
    push byte 0 
    push byte 202
    jmp isr_common_stub 

isr203:
    push byte 0 
    push byte 203
    jmp isr_common_stub 

isr204:
    push byte 0 
    push byte 204
    jmp isr_common_stub 

isr205:
    push byte 0 
    push byte 205
    jmp isr_common_stub 

isr206:
    push byte 0 
    push byte 206
    jmp isr_common_stub 

isr207:
    push byte 0 
    push byte 207
    jmp isr_common_stub 

isr208:
    push byte 0 
    push byte 208
    jmp isr_common_stub 

isr209:
    push byte 0 
    push byte 209
    jmp isr_common_stub 

isr210:
    push byte 0 
    push byte 210
    jmp isr_common_stub 

isr211:
    push byte 0 
    push byte 211
    jmp isr_common_stub 

isr212:
    push byte 0 
    push byte 212
    jmp isr_common_stub 

isr213:
    push byte 0 
    push byte 213
    jmp isr_common_stub 

isr214:
    push byte 0 
    push byte 214
    jmp isr_common_stub 

isr215:
    push byte 0 
    push byte 215
    jmp isr_common_stub 

isr216:
    push byte 0 
    push byte 216
    jmp isr_common_stub 

isr217:
    push byte 0 
    push byte 217
    jmp isr_common_stub 

isr218:
    push byte 0 
    push byte 218
    jmp isr_common_stub 

isr219:
    push byte 0 
    push byte 219
    jmp isr_common_stub 

isr220:
    push byte 0 
    push byte 220
    jmp isr_common_stub 

isr221:
    push byte 0 
    push byte 221
    jmp isr_common_stub 

isr222:
    push byte 0 
    push byte 222
    jmp isr_common_stub 

isr223:
    push byte 0 
    push byte 223
    jmp isr_common_stub 

isr224:
    push byte 0 
    push byte 224
    jmp isr_common_stub 

isr225:
    push byte 0 
    push byte 225
    jmp isr_common_stub 

isr226:
    push byte 0 
    push byte 226
    jmp isr_common_stub 

isr227:
    push byte 0 
    push byte 227
    jmp isr_common_stub 

isr228:
    push byte 0 
    push byte 228
    jmp isr_common_stub 

isr229:
    push byte 0 
    push byte 229
    jmp isr_common_stub 

isr230:
    push byte 0 
    push byte 230
    jmp isr_common_stub 

isr231:
    push byte 0 
    push byte 231
    jmp isr_common_stub 

isr232:
    push byte 0 
    push byte 232
    jmp isr_common_stub 

isr233:
    push byte 0 
    push byte 233
    jmp isr_common_stub 

isr234:
    push byte 0 
    push byte 234
    jmp isr_common_stub 

isr235:
    push byte 0 
    push byte 235
    jmp isr_common_stub 

isr236:
    push byte 0 
    push byte 236
    jmp isr_common_stub 

isr237:
    push byte 0 
    push byte 237
    jmp isr_common_stub 

isr238:
    push byte 0 
    push byte 238
    jmp isr_common_stub 

isr239:
    push byte 0 
    push byte 239
    jmp isr_common_stub 

isr240:
    push byte 0 
    push byte 240
    jmp isr_common_stub 

isr241:
    push byte 0 
    push byte 241
    jmp isr_common_stub 

isr242:
    push byte 0 
    push byte 242
    jmp isr_common_stub 

isr243:
    push byte 0 
    push byte 243
    jmp isr_common_stub 

isr244:
    push byte 0 
    push byte 244
    jmp isr_common_stub 

isr245:
    push byte 0 
    push byte 245
    jmp isr_common_stub 

isr246:
    push byte 0 
    push byte 246
    jmp isr_common_stub 

isr247:
    push byte 0 
    push byte 247
    jmp isr_common_stub 

isr248:
    push byte 0 
    push byte 248
    jmp isr_common_stub 

isr249:
    push byte 0 
    push byte 249
    jmp isr_common_stub 

isr250:
    push byte 0 
    push byte 250
    jmp isr_common_stub 

isr251:
    push byte 0 
    push byte 251
    jmp isr_common_stub 

isr252:
    push byte 0 
    push byte 252
    jmp isr_common_stub 

isr253:
    push byte 0 
    push byte 253
    jmp isr_common_stub 

isr254:
    push byte 0 
    push byte 254
    jmp isr_common_stub 

isr255:
    push byte 0 
    push byte 255
    jmp isr_common_stub 
