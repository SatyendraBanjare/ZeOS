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
    push word 0
    push word 0
    jmp isr_common_stub

; 1: Debug Exception
isr1:
    push word 0
    push word 1
    jmp isr_common_stub

; 2: Non Maskable Interrupt Exception
isr2:
    push word 0
    push word 2
    jmp isr_common_stub

; 3: Int 3 Exception
isr3:
    push word 0
    push word 3
    jmp isr_common_stub

; 4: INTO Exception
isr4:
    push word 0
    push word 4
    jmp isr_common_stub

; 5: Out of Bounds Exception
isr5:
    push word 0
    push word 5
    jmp isr_common_stub

; 6: Invalid Opcode Exception
isr6:
    push word 0
    push word 6
    jmp isr_common_stub

; 7: Coprocessor Not Available Exception
isr7:
    push word 0
    push word 7
    jmp isr_common_stub

; 8: Double Fault Exception (With Error Code!)
isr8:
    push word 8
    jmp isr_common_stub

; 9: Coprocessor Segment Overrun Exception
isr9:
    push word 0
    push word 9
    jmp isr_common_stub

; 10: Bad TSS Exception (With Error Code!)
isr10:
    push word 10
    jmp isr_common_stub

; 11: Segment Not Present Exception (With Error Code!)
isr11:
    push word 11
    jmp isr_common_stub

; 12: Stack Fault Exception (With Error Code!)
isr12:
    push word 12
    jmp isr_common_stub

; 13: General Protection Fault Exception (With Error Code!)
isr13:
    push word 13
    jmp isr_common_stub

; 14: Page Fault Exception (With Error Code!)
isr14:
    push word 14
    jmp isr_common_stub

; 15: Reserved Exception
isr15:
    push word 0
    push word 15
    jmp isr_common_stub

; 16: Floating Point Exception
isr16:
    push word 0
    push word 16
    jmp isr_common_stub

; 17: Alignment Check Exception
isr17:
    push word 0
    push word 17
    jmp isr_common_stub

; 18: Machine Check Exception
isr18:
    push word 0
    push word 18
    jmp isr_common_stub

; 19: Reserved
isr19:
    push word 0
    push word 19
    jmp isr_common_stub

; 20: Reserved
isr20:
    push word 0
    push word 20
    jmp isr_common_stub

; 21: Reserved
isr21:
    push word 0
    push word 21
    jmp isr_common_stub

; 22: Reserved
isr22:
    push word 0
    push word 22
    jmp isr_common_stub

; 23: Reserved
isr23:
    push word 0
    push word 23
    jmp isr_common_stub

; 24: Reserved
isr24:
    push word 0
    push word 24
    jmp isr_common_stub

; 25: Reserved
isr25:
    push word 0
    push word 25
    jmp isr_common_stub

; 26: Reserved
isr26:
    push word 0
    push word 26
    jmp isr_common_stub

; 27: Reserved
isr27:
    push word 0
    push word 27
    jmp isr_common_stub

; 28: Reserved
isr28:
    push word 0
    push word 28
    jmp isr_common_stub

; 29: Reserved
isr29:
    push word 0
    push word 29
    jmp isr_common_stub

; 30: Reserved
isr30:
    push word 0
    push word 30
    jmp isr_common_stub

; 31: Reserved
isr31:
    push word 0
    push word 31
    jmp isr_common_stub

; IRQ handlers
irq0:
	push word 0
	push word 32
	jmp irq_common_stub

irq1:
	push word 1
	push word 33
	jmp irq_common_stub

irq2:
	push word 2
	push word 34
	jmp irq_common_stub

irq3:
	push word 3
	push word 35
	jmp irq_common_stub

irq4:
	push word 4
	push word 36
	jmp irq_common_stub

irq5:
	push word 5
	push word 37
	jmp irq_common_stub

irq6:
	push word 6
	push word 38
	jmp irq_common_stub

irq7:
	push word 7
	push word 39
	jmp irq_common_stub

irq8:
	push word 8
	push word 40
	jmp irq_common_stub

irq9:
	push word 9
	push word 41
	jmp irq_common_stub

irq10:
	push word 10
	push word 42
	jmp irq_common_stub

irq11:
	push word 11
	push word 43
	jmp irq_common_stub

irq12:
	push word 12
	push word 44
	jmp irq_common_stub

irq13:
	push word 13
	push word 45
	jmp irq_common_stub

irq14:
	push word 14
	push word 46
	jmp irq_common_stub

irq15:
	push word 15
	push word 47
	jmp irq_common_stub


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

isr48:
    push word 0 
    push word 48
    jmp isr_common_stub 

isr49:
    push word 0 
    push word 49
    jmp isr_common_stub 

isr50:
    push word 0 
    push word 50
    jmp isr_common_stub 

isr51:
    push word 0 
    push word 51
    jmp isr_common_stub 

isr52:
      push word 0 
      push word 52
    jmp isr_common_stub 

isr53:
    push word 0 
    push word 53
    jmp isr_common_stub 

isr54:
    push word 0 
    push word 54
    jmp isr_common_stub 

isr55:
    push word 0 
    push word 55
    jmp isr_common_stub 

isr56:
    push word 0 
    push word 56
    jmp isr_common_stub 

isr57:
    push word 0 
    push word 57
    jmp isr_common_stub 

isr58:
    push word 0 
    push word 58
    jmp isr_common_stub 

isr59:
    push word 0 
    push word 59
    jmp isr_common_stub 

isr60:
    push word 0 
    push word 60
    jmp isr_common_stub 

isr61:
    push word 0 
    push word 61
    jmp isr_common_stub 

isr62:
    push word 0 
    push word 62
    jmp isr_common_stub 

isr63:
    push word 0 
    push word 63
    jmp isr_common_stub 

isr64:
    push word 0 
    push word 64
    jmp isr_common_stub 

isr65:
    push word 0 
    push word 65
    jmp isr_common_stub 

isr66:
    push word 0 
    push word 66
    jmp isr_common_stub 

isr67:
    push word 0 
    push word 67
    jmp isr_common_stub 

isr68:
    push word 0 
    push word 68
    jmp isr_common_stub 

isr69:
    push word 0 
    push word 69
    jmp isr_common_stub 

isr70:
    push word 0 
    push word 70
    jmp isr_common_stub 

isr71:
    push word 0 
    push word 71
    jmp isr_common_stub 

isr72:
    push word 0 
    push word 72
    jmp isr_common_stub 

isr73:
    push word 0 
    push word 73
    jmp isr_common_stub 

isr74:
    push word 0 
    push word 74
    jmp isr_common_stub 

isr75:
    push word 0 
    push word 75
    jmp isr_common_stub 

isr76:
    push word 0 
    push word 76
    jmp isr_common_stub 

isr77:
    push word 0 
    push word 77
    jmp isr_common_stub 

isr78:
    push word 0 
    push word 78
    jmp isr_common_stub 

isr79:
    push word 0 
    push word 79
    jmp isr_common_stub 

isr80:
    push word 0 
    push word 80
    jmp isr_common_stub 

isr81:
    push word 0 
    push word 81
    jmp isr_common_stub 

isr82:
    push word 0 
    push word 82
    jmp isr_common_stub 

isr83:
    push word 0 
    push word 83
    jmp isr_common_stub 

isr84:
    push word 0 
    push word 84
    jmp isr_common_stub 

isr85:
    push word 0 
    push word 85
    jmp isr_common_stub 

isr86:
    push word 0 
    push word 86
    jmp isr_common_stub 

isr87:
    push word 0 
    push word 87
    jmp isr_common_stub 

isr88:
    push word 0 
    push word 88
    jmp isr_common_stub 

isr89:
    push word 0 
    push word 89
    jmp isr_common_stub 

isr90:
    push word 0 
    push word 90
    jmp isr_common_stub 

isr91:
    push word 0 
    push word 91
    jmp isr_common_stub 

isr92:
    push word 0 
    push word 92
    jmp isr_common_stub 

isr93:
    push word 0 
    push word 93
    jmp isr_common_stub 

isr94:
    push word 0 
    push word 94
    jmp isr_common_stub 

isr95:
    push word 0 
    push word 95
    jmp isr_common_stub 

isr96:
    push word 0 
    push word 96
    jmp isr_common_stub 

isr97:
    push word 0 
    push word 97
    jmp isr_common_stub 

isr98:
    push word 0 
    push word 98
    jmp isr_common_stub 

isr99:
    push word 0 
    push word 99
    jmp isr_common_stub 

isr100:
    push word 0 
    push word 100
    jmp isr_common_stub 

isr101:
    push word 0 
    push word 101
    jmp isr_common_stub 

isr102:
    push word 0 
    push word 102
    jmp isr_common_stub 

isr103:
    push word 0 
    push word 103
    jmp isr_common_stub 

isr104:
    push word 0 
    push word 104
    jmp isr_common_stub 

isr105:
    push word 0 
    push word 105
    jmp isr_common_stub 

isr106:
    push word 0 
    push word 106
    jmp isr_common_stub 

isr107:
    push word 0 
    push word 107
    jmp isr_common_stub 

isr108:
    push word 0 
    push word 108
    jmp isr_common_stub 

isr109:
    push word 0 
    push word 109
    jmp isr_common_stub 

isr110:
    push word 0 
    push word 110
    jmp isr_common_stub 

isr111:
    push word 0 
    push word 111
    jmp isr_common_stub 

isr112:
    push word 0 
    push word 112
    jmp isr_common_stub 

isr113:
    push word 0 
    push word 113
    jmp isr_common_stub 

isr114:
    push word 0 
    push word 114
    jmp isr_common_stub 

isr115:
    push word 0 
    push word 115
    jmp isr_common_stub 

isr116:
    push word 0 
    push word 116
    jmp isr_common_stub 

isr117:
    push word 0 
    push word 117
    jmp isr_common_stub 

isr118:
    push word 0 
    push word 118
    jmp isr_common_stub 

isr119:
    push word 0 
    push word 119
    jmp isr_common_stub 

isr120:
    push word 0 
    push word 120
    jmp isr_common_stub 

isr121:
    push word 0 
    push word 121
    jmp isr_common_stub 

isr122:
    push word 0 
    push word 122
    jmp isr_common_stub 

isr123:
    push word 0 
    push word 123
    jmp isr_common_stub 

isr124:
    push word 0 
    push word 124
    jmp isr_common_stub 

isr125:
    push word 0 
    push word 125
    jmp isr_common_stub 

isr126:
    push word 0 
    push word 126
    jmp isr_common_stub 

isr127:
    push word 0 
    push word 127
    jmp isr_common_stub 

isr128:
    push word 0 
    push word 128
    jmp isr_common_stub 

isr129:
    push word 0 
    push word 129
    jmp isr_common_stub 

isr130:
    push word 0 
    push word 130
    jmp isr_common_stub 

isr131:
    push word 0 
    push word 131
    jmp isr_common_stub 

isr132:
    push word 0 
    push word 132
    jmp isr_common_stub 

isr133:
    push word 0 
    push word 133
    jmp isr_common_stub 

isr134:
    push word 0 
    push word 134
    jmp isr_common_stub 

isr135:
    push word 0 
    push word 135
    jmp isr_common_stub 

isr136:
    push word 0 
    push word 136
    jmp isr_common_stub 

isr137:
    push word 0 
    push word 137
    jmp isr_common_stub 

isr138:
    push word 0 
    push word 138
    jmp isr_common_stub 

isr139:
    push word 0 
    push word 139
    jmp isr_common_stub 

isr140:
    push word 0 
    push word 140
    jmp isr_common_stub 

isr141:
    push word 0 
    push word 141
    jmp isr_common_stub 

isr142:
    push word 0 
    push word 142
    jmp isr_common_stub 

isr143:
    push word 0 
    push word 143
    jmp isr_common_stub 

isr144:
    push word 0 
    push word 144
    jmp isr_common_stub 

isr145:
    push word 0 
    push word 145
    jmp isr_common_stub 

isr146:
    push word 0 
    push word 146
    jmp isr_common_stub 

isr147:
    push word 0 
    push word 147
    jmp isr_common_stub 

isr148:
    push word 0 
    push word 148
    jmp isr_common_stub 

isr149:
    push word 0 
    push word 149
    jmp isr_common_stub 

isr150:
    push word 0 
    push word 150
    jmp isr_common_stub 

isr151:
    push word 0 
    push word 151
    jmp isr_common_stub 

isr152:
    push word 0 
    push word 152
    jmp isr_common_stub 

isr153:
    push word 0 
    push word 153
    jmp isr_common_stub 

isr154:
    push word 0 
    push word 154
    jmp isr_common_stub 

isr155:
    push word 0 
    push word 155
    jmp isr_common_stub 

isr156:
    push word 0 
    push word 156
    jmp isr_common_stub 

isr157:
    push word 0 
    push word 157
    jmp isr_common_stub 

isr158:
    push word 0 
    push word 158
    jmp isr_common_stub 

isr159:
    push word 0 
    push word 159
    jmp isr_common_stub 

isr160:
    push word 0 
    push word 160
    jmp isr_common_stub 

isr161:
    push word 0 
    push word 161
    jmp isr_common_stub 

isr162:
    push word 0 
    push word 162
    jmp isr_common_stub 

isr163:
    push word 0 
    push word 163
    jmp isr_common_stub 

isr164:
    push word 0 
    push word 164
    jmp isr_common_stub 

isr165:
    push word 0 
    push word 165
    jmp isr_common_stub 

isr166:
    push word 0 
    push word 166
    jmp isr_common_stub 

isr167:
    push word 0 
    push word 167
    jmp isr_common_stub 

isr168:
    push word 0 
    push word 168
    jmp isr_common_stub 

isr169:
    push word 0 
    push word 169
    jmp isr_common_stub 

isr170:
    push word 0 
    push word 170
    jmp isr_common_stub 

isr171:
    push word 0 
    push word 171
    jmp isr_common_stub 

isr172:
    push word 0 
    push word 172
    jmp isr_common_stub 

isr173:
    push word 0 
    push word 173
    jmp isr_common_stub 

isr174:
    push word 0 
    push word 174
    jmp isr_common_stub 

isr175:
    push word 0 
    push word 175
    jmp isr_common_stub 

isr176:
    push word 0 
    push word 176
    jmp isr_common_stub 

isr177:
    push word 0 
    push word 177
    jmp isr_common_stub 

isr178:
    push word 0 
    push word 178
    jmp isr_common_stub 

isr179:
    push word 0 
    push word 179
    jmp isr_common_stub 

isr180:
    push word 0 
    push word 180
    jmp isr_common_stub 

isr181:
    push word 0 
    push word 181
    jmp isr_common_stub 

isr182:
    push word 0 
    push word 182
    jmp isr_common_stub 

isr183:
    push word 0 
    push word 183
    jmp isr_common_stub 

isr184:
    push word 0 
    push word 184
    jmp isr_common_stub 

isr185:
    push word 0 
    push word 185
    jmp isr_common_stub 

isr186:
    push word 0 
    push word 186
    jmp isr_common_stub 

isr187:
    push word 0 
    push word 187
    jmp isr_common_stub 

isr188:
    push word 0 
    push word 188
    jmp isr_common_stub 

isr189:
    push word 0 
    push word 189
    jmp isr_common_stub 

isr190:
    push word 0 
    push word 190
    jmp isr_common_stub 

isr191:
    push word 0 
    push word 191
    jmp isr_common_stub 

isr192:
    push word 0 
    push word 192
    jmp isr_common_stub 

isr193:
    push word 0 
    push word 193
    jmp isr_common_stub 

isr194:
    push word 0 
    push word 194
    jmp isr_common_stub 

isr195:
    push word 0 
    push word 195
    jmp isr_common_stub 

isr196:
    push word 0 
    push word 196
    jmp isr_common_stub 

isr197:
    push word 0 
    push word 197
    jmp isr_common_stub 

isr198:
    push word 0 
    push word 198
    jmp isr_common_stub 

isr199:
    push word 0 
    push word 199
    jmp isr_common_stub 

isr200:
    push word 0 
    push word 200
    jmp isr_common_stub 

isr201:
    push word 0 
    push word 201
    jmp isr_common_stub 

isr202:
    push word 0 
    push word 202
    jmp isr_common_stub 

isr203:
    push word 0 
    push word 203
    jmp isr_common_stub 

isr204:
    push word 0 
    push word 204
    jmp isr_common_stub 

isr205:
    push word 0 
    push word 205
    jmp isr_common_stub 

isr206:
    push word 0 
    push word 206
    jmp isr_common_stub 

isr207:
    push word 0 
    push word 207
    jmp isr_common_stub 

isr208:
    push word 0 
    push word 208
    jmp isr_common_stub 

isr209:
    push word 0 
    push word 209
    jmp isr_common_stub 

isr210:
    push word 0 
    push word 210
    jmp isr_common_stub 

isr211:
    push word 0 
    push word 211
    jmp isr_common_stub 

isr212:
    push word 0 
    push word 212
    jmp isr_common_stub 

isr213:
    push word 0 
    push word 213
    jmp isr_common_stub 

isr214:
    push word 0 
    push word 214
    jmp isr_common_stub 

isr215:
    push word 0 
    push word 215
    jmp isr_common_stub 

isr216:
    push word 0 
    push word 216
    jmp isr_common_stub 

isr217:
    push word 0 
    push word 217
    jmp isr_common_stub 

isr218:
    push word 0 
    push word 218
    jmp isr_common_stub 

isr219:
    push word 0 
    push word 219
    jmp isr_common_stub 

isr220:
    push word 0 
    push word 220
    jmp isr_common_stub 

isr221:
    push word 0 
    push word 221
    jmp isr_common_stub 

isr222:
    push word 0 
    push word 222
    jmp isr_common_stub 

isr223:
    push word 0 
    push word 223
    jmp isr_common_stub 

isr224:
    push word 0 
    push word 224
    jmp isr_common_stub 

isr225:
    push word 0 
    push word 225
    jmp isr_common_stub 

isr226:
    push word 0 
    push word 226
    jmp isr_common_stub 

isr227:
    push word 0 
    push word 227
    jmp isr_common_stub 

isr228:
    push word 0 
    push word 228
    jmp isr_common_stub 

isr229:
    push word 0 
    push word 229
    jmp isr_common_stub 

isr230:
    push word 0 
    push word 230
    jmp isr_common_stub 

isr231:
    push word 0 
    push word 231
    jmp isr_common_stub 

isr232:
    push word 0 
    push word 232
    jmp isr_common_stub 

isr233:
    push word 0 
    push word 233
    jmp isr_common_stub 

isr234:
    push word 0 
    push word 234
    jmp isr_common_stub 

isr235:
    push word 0 
    push word 235
    jmp isr_common_stub 

isr236:
    push word 0 
    push word 236
    jmp isr_common_stub 

isr237:
    push word 0 
    push word 237
    jmp isr_common_stub 

isr238:
    push word 0 
    push word 238
    jmp isr_common_stub 

isr239:
    push word 0 
    push word 239
    jmp isr_common_stub 

isr240:
    push word 0 
    push word 240
    jmp isr_common_stub 

isr241:
    push word 0 
    push word 241
    jmp isr_common_stub 

isr242:
    push word 0 
    push word 242
    jmp isr_common_stub 

isr243:
    push word 0 
    push word 243
    jmp isr_common_stub 

isr244:
    push word 0 
    push word 244
    jmp isr_common_stub 

isr245:
    push word 0 
    push word 245
    jmp isr_common_stub 

isr246:
    push word 0 
    push word 246
    jmp isr_common_stub 

isr247:
    push word 0 
    push word 247
    jmp isr_common_stub 

isr248:
    push word 0 
    push word 248
    jmp isr_common_stub 

isr249:
    push word 0 
    push word 249
    jmp isr_common_stub 

isr250:
    push word 0 
    push word 250
    jmp isr_common_stub 

isr251:
    push word 0 
    push word 251
    jmp isr_common_stub 

isr252:
    push word 0 
    push word 252
    jmp isr_common_stub 

isr253:
    push word 0 
    push word 253
    jmp isr_common_stub 

isr254:
    push word 0 
    push word 254
    jmp isr_common_stub 

isr255:
    push word 0 
    push word 255
    jmp isr_common_stub 
