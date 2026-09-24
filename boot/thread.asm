[GLOBAL switch_context]
[GLOBAL thread_entry_asm]
[EXTERN thread_exit]

; void switch_context(uint32_t *old_esp, uint32_t new_esp)
; Saves the callee-saved registers on the current stack, stores esp in *old_esp,
; then loads new_esp and restores the registers saved there.
switch_context:
  mov eax, [esp + 4]
  mov edx, [esp + 8]
  push ebp
  push ebx
  push esi
  push edi
  mov [eax], esp
  mov esp, edx
  pop edi
  pop esi
  pop ebx
  pop ebp
  ret

; First code a new thread runs (switch_context "returns" here).
; ebx = entry function, esi = argument. Interrupts are enabled first because a
; new thread may be started from inside the timer interrupt.
thread_entry_asm:
  sti
  push esi
  call ebx
  add esp, 4
  call thread_exit
.hang:
  hlt
  jmp .hang
