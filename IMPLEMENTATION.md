# ZeOS – Implementation Details

This document describes, feature by feature, what has been implemented in ZeOS and how it works. It follows the **Progress** list in `README.md`, then covers the pieces that were built along the way but are not on that list (serial logging, PIC/timer, the shell, the build system).

Conventions used below:

- **Status** says what is actually true of the code today.
- **Verified** says how it was checked. Most checking was done by booting `zeos.bin` directly in QEMU (`qemu-system-i386 -kernel zeos.bin`) and driving it with QEMU monitor `sendkey`. The full GRUB + ISO path could not be tested on the development machine used for the latest work (no `mkisofs`/`genisoimage` installed), so anything GRUB-specific is marked as such.
- Addresses are for the 32-bit (i386) build.

---

## Contents

1. [Big picture](#1-big-picture)
2. [Boot sequence](#2-boot-sequence)
3. [Grub Multiboot log](#3-grub-multiboot-log)
4. [GDT setup](#4-gdt-setup)
5. [IDT & ISR setup](#5-idt--isr-setup)
6. [PIC, timer and serial port](#6-pic-timer-and-serial-port)
7. [Terminal](#7-terminal)
8. [Basic keyboard input and line editing](#8-basic-keyboard-input-and-line-editing)
9. [Higher-half kernel](#9-higher-half-kernel)
10. [Paging](#10-paging)
11. [Heap](#11-heap)
12. [Multitasking (kernel threads)](#12-multitasking-kernel-threads)
13. [InitRD](#13-initrd)
14. [VFS](#14-vfs)
15. [The shell and its commands](#15-the-shell-and-its-commands)
16. [Build system and ISO creation](#16-build-system-and-iso-creation)
17. [Not implemented yet](#17-not-implemented-yet)
18. [Known issues and README corrections](#18-known-issues-and-readme-corrections)

---

## 1. Big picture

ZeOS is a 32-bit x86 hobby operating system (currently a kernel with a shell, and no user mode yet), loaded by GRUB (legacy, via `stage2_eltorito`) using the Multiboot 1 protocol. It is a **monolithic, single-address-space, ring-0-only** kernel:

- The kernel is linked at virtual `0xC0100000` and loaded at physical `0x100000` (1 MB): a higher-half kernel.
- Everything (kernel, drivers, the shell, kernel threads) runs in ring 0 and shares one address space.
- Output goes to the VGA text console (80×24) and, for logs, to the COM1 serial port.
- Input comes from the PS/2 keyboard via IRQ1.

### Source layout

| Path | Contents |
|---|---|
| `boot/boot.asm` | Multiboot header, early paging, jump to the higher half, stack setup, call to `kernel_main` |
| `boot/gdt.asm`, `boot/idt.asm` | `lgdt` / `lidt` wrappers |
| `boot/interrupt.asm` | 256 generated interrupt stubs + the common stub |
| `boot/thread.asm` | Thread context switch and thread entry trampoline |
| `boot/usermode.asm` | Only `get_current_stack_pointer` (despite the name) |
| `conf/linker.ld` | Linker script (higher-half layout) |
| `conf/menu.lst` | GRUB legacy menu (kernel + initrd module) |
| `kernel/main.c` | `kernel_main`: initialization order |
| `kernel/cpu/` | GDT, IDT, ISR dispatch, PIC, timer, serial, port I/O, threads |
| `kernel/cpu/memory/` | Heap allocator, page directory / physical-page code |
| `kernel/drivers/keyboard.c` | Keyboard driver and line editor |
| `kernel/terminal/terminal.c` | VGA text output |
| `kernel/multiboot/` | Multiboot info parsing and logging |
| `kernel/fs/fs_new.c`, `kernel/fs/initrd.c` | VFS and initrd loader |
| `kernel/shell/shell.c` | Command interpreter, header bar, demo threads |
| `scripts/make_initrd.py`, `initrd/` | initrd image builder and its source files |

---

## 2. Boot sequence

What happens from power-on to the shell prompt:

1. **BIOS → GRUB (stage2_eltorito).** GRUB reads `menu.lst`, loads `/boot/zeos.bin` at physical 1 MB and the module `/module/initrd.img` just after it. It jumps to the Multiboot entry with `EAX = 0x2BADB002` and `EBX =` physical address of the Multiboot info structure.
2. **`loader` in `boot/boot.asm`** (runs with paging **off**, at physical addresses):
   - saves `EAX`/`EBX`,
   - builds a page table and directory (see [Higher-half kernel](#9-higher-half-kernel)),
   - turns on paging (`CR0.PG`) and 4 MB pages (`CR4.PSE`),
   - jumps to `higher_half_loader` at a virtual address above 3 GB,
   - removes the temporary identity mapping, sets `ESP` to the top of the 1 MB kernel stack,
   - pushes the boot information as arguments and calls `kernel_main`.
3. **`kernel_main`** (`kernel/main.c`):
   1. logs kernel addresses, page directory/table addresses, GRUB magic, and the full Multiboot info to the serial port,
   2. `serial_init()`,
   3. `init_gdt()`, `init_idt()`,
   4. `threads_init()` (adopts the boot context as thread 0 *before* the timer can fire),
   5. `init_timer(50)` then `pic_init()` (this executes `sti`, so interrupts are live from here),
   6. `clear_screen_full()`, `init_shell()` (welcome banner, header bar, VFS creation, default directories),
   7. `initrd_load()` (unpacks the initrd into `/initrd`),
   8. loops on `hlt` forever: `main` becomes the idle thread. All further work is interrupt-driven.

---

## 3. Grub Multiboot log

**Status:** done. **Files:** `kernel/multiboot/multiboot_util.c`, `kernel/include/multiboot/multiboot.h`, `kernel/main.c`.

The bootloader hands the kernel a `struct multiboot_info` (physical address in `EBX`). `boot.asm` adds `0xC0000000` to it and passes it as the 4th argument of `kernel_main`. `print_multiboot_info()` then dumps everything useful through `print_log()` (serial), which is what ends up in `log/log.txt` under `make run`.

What is printed:

- **Header values:** `flags`, `mem_lower` / `mem_upper` (KB), `boot_device`, `cmdline`, `mods_count`, `mods_addr`.
- **Flag decoding:** one line per set flag (`MULTIBOOT_INFO_MEMORY`, `..._BOOTDEV`, `..._CMDLINE`, `..._MODS`, `..._MEM_MAP`, ...).
- **Memory map** (`print_memory_map`): each `multiboot_memory_map_t` entry with base address, length, size and type (`1 MULTIBOOT_MEMORY_AVAILABLE`, `2 MULTIBOOT_MEMORY_RESERVED`), so you can see what RAM is usable.
- **Module info** (`print_module_info`): for every GRUB module, its command line (`/module/initrd.img`), and its start/end (as virtual addresses).
- **Cmdline:** the kernel command line string, if GRUB provided one.

`p_to_v()` converts physical → virtual by adding `KERNEL_VIRTUAL_BASE` (`0xC0000000`). All pointers GRUB gives (info struct, cmdline, module list, module strings) are physical, so every dereference goes through it.

**Caveat found while testing:** these dereferences only work if the target physical page is mapped. The original boot code mapped only the kernel image; it now maps 4 MB (see [Paging](#10-paging)). Booting with `qemu -kernel`, QEMU places the command line and initrd immediately after the kernel, which was exactly the unmapped region that used to page-fault.

---

## 4. GDT setup

**Status:** done (flat model, kernel segments only). **Files:** `kernel/cpu/gdt.c`, `boot/gdt.asm`.

The GDT tells the CPU which memory "segments" exist. ZeOS uses the simplest possible setup: a **flat memory model** with three descriptors.

| Index | Selector | Access byte | Flags/limit | Purpose |
|---|---|---|---|---|
| 0 | `0x00` | `0` | `0` | Mandatory null descriptor |
| 1 | `0x08` | `0b10011010` (`0x9A`) | `0xCF` | Kernel **code**: present, ring 0, executable, readable |
| 2 | `0x10` | `0b10010010` (`0x92`) | `0xCF` | Kernel **data**: present, ring 0, writable |

Both segments have base `0` and limit `0xFFFFF` with 4 KB granularity (flags nibble `0xC` = granularity + 32-bit), so each covers the entire 4 GB address space. Segmentation is effectively bypassed and protection is left to paging.

`init_gdt()` fills `struct segment_descriptor_t gdt[3]` (a packed 8-byte structure with the split base/limit fields the hardware wants), builds the 6-byte GDT pointer (`size - 1`, `offset`), and calls `load_gdt()`, a two-instruction assembly function (`mov edx,[esp+4]` / `lgdt [edx]`). GRUB already loaded `CS=0x08` and the data registers `=0x10`, so the segment registers are not reloaded here.

There is **no TSS and no ring-3 descriptors**, which is why user mode is not possible yet.

---

## 5. IDT & ISR setup

**Status:** done. **Files:** `kernel/cpu/idt.c`, `boot/idt.asm`, `boot/interrupt.asm`, `kernel/cpu/isr.c`, `kernel/include/cpu/state.h`.

### The IDT

`init_idt()` fills all **256** gates of `struct interrupt_descriptor_t idt[256]`. Every gate is:

- selector `0x08` (kernel code),
- type/attribute `0x8E` = present, ring 0, **32-bit interrupt gate**. An interrupt gate makes the CPU clear `IF` on entry, so interrupt handlers run with interrupts disabled. (This property is what the thread scheduler relies on.)
- offset = address of the matching assembly stub, split into low/high 16 bits.

`load_idt()` (`boot/idt.asm`) executes `lidt`.

### The assembly stubs

`boot/interrupt.asm` (generated, ~1500 lines) contains `interrupt_handler_0` … `interrupt_handler_255`. Each stub pushes a dummy error code (`push dword 0`, except vector 14 where the CPU already pushed one) and its vector number, then jumps to `common_interrupt_handler`, which:

1. `pusha` (saves EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI),
2. reads `CR2` (the faulting address, meaningful for page faults) and pushes it,
3. loads `DS/ES/FS/GS` with the kernel data selector `0x10`,
4. pushes `ESP` and calls the C function `interrupt_handler` (with `cld`, as the System V ABI requires),
5. on return pops those values, restores segment registers, `popa`, drops the vector number and error code (`add esp, 8`) and executes `iret`.

`struct cpu_state` (`state.h`) is laid out to mirror what is on the stack at that point: `cr2`, `ds`, general registers, `int_no`, `err_code`, then the CPU-pushed `eip, cs, eflags, esp, ss`.

### The C dispatcher (`interrupt_handler` in `isr.c`)

- Logs the interrupt number and error code to the serial port on every interrupt.
- `switch (int_no)`:
  - **32** (IRQ0, timer) → `timer_callback()`
  - **33** (IRQ1, keyboard) → `keyboard_callback()`
  - **14** (page fault) → logs the faulting address (`CR2`) and decodes the error code bits (present / write / user / reserved / instruction fetch), then spins forever
  - anything else → logs "Unable to handle interrupt"
- Sends End-Of-Interrupt to the PIC (`pic_acknowledge()`).
- For the timer only, calls `scheduler_tick()` **after** the EOI, so a context switch can never leave the PIC waiting for an acknowledgement.

The known weaknesses of this area are listed in [section 18](#18-known-issues-and-readme-corrections).

---

## 6. PIC, timer and serial port

These are not separate README items, but everything else depends on them.

### PIC (`kernel/cpu/pic.c`)

The legacy 8259 PICs power up mapping IRQ0–7 onto CPU exception vectors 8–15, which collide with real exceptions. `pic_init()` performs the standard four-word (ICW1–ICW4) initialization sequence to **remap the master PIC to vectors 0x20–0x27 and the slave to 0x28–0x2F**, wires the slave to the master's IRQ2 line, selects 8086 mode, and then sets the interrupt masks so IRQ0 (timer), IRQ1 (keyboard) and IRQ2 (cascade) are enabled. It then executes `sti`. `pic_acknowledge()` writes `0x20` (EOI) to the master's command port.

### Timer (`kernel/cpu/timer.c`)

`init_timer(50)` programs PIT channel 0: it sends command `0x36` (channel 0, lo/hi byte, square-wave mode) to port `0x43`, then the divisor `1193180 / 50` to port `0x40`, giving a **50 Hz tick (20 ms)**. `timer_callback()` increments the global `tick` counter and redraws the uptime in the header bar.

Note: the uptime number in the header is derived from the high 32 bits of the CPU's time-stamp counter (`rdtsc`), not from `tick`, so it is only roughly proportional to seconds. The scheduler and `thread_sleep` use `tick`, which is accurate.

### Serial port (`kernel/cpu/serial.c`)

`serial_init()` configures **COM1 (`0x3F8`)**: baud divisor (115200 / divisor), 8 data bits, no parity, one stop bit, FIFO enabled with 14-byte threshold, modem control ready. `print_log()` writes characters to it (waiting for the transmit FIFO to drain). When you run `make run`, QEMU is started with `-serial file:log/log.txt`, so all kernel logging lands in that file, which is the primary debugging tool because the screen is used by the shell.

### Port I/O (`kernel/cpu/util.c`)

`outb`/`outw`/`inb`/`inw` are inline-assembly wrappers around the x86 `out` / `in` instructions.

---

## 7. Terminal

**Status:** done. **Files:** `kernel/terminal/terminal.c`, `kernel/include/terminal/terminal.h`.

The terminal writes directly to **VGA text-mode memory** at physical `0xB8000`, reached at virtual `0xC00B8000` through the higher-half mapping. Each on-screen cell is two bytes: the ASCII character, then an attribute byte (foreground/background colour). The screen is `VGA_WIDTH = 80` columns.

### Output primitives

- `print_char(c, col, row, attr)` is the innermost routine. Given a position (or "current cursor" if negative), it writes the cell, advances the offset, handles `'\n'` (jump to column 0 of the next row) and `0x08` (backspace: blank the cell), and **scrolls** when the offset passes the last row.
- `zprint()`, `zprint_at()`, `zprint_new_line()` print strings with the command colour (`0x0F`, white on black) or the prompt colour (`0x0A`, green).
- `zprint_int()` prints a decimal number.
- `print_char_hf()` writes header/footer cells without touching the cursor.

### Terminal scrolling

When the cursor offset moves past the end of the visible area, `print_char` copies each line up one row using `memory_copy` (row *i+1* → row *i*), blanks the last line, and moves the offset back by one row. Because the copy starts at row 1, the header bar on row 0 is never scrolled away.

### Cursor

The hardware blinking cursor is moved by writing the offset (high byte to register 14, low byte to register 15) through VGA control ports `0x3D4` / `0x3D5` (`set_cursor_offset`); `get_cursor_offset` reads it back the same way. `zprint_left()` / `zprint_right()` just add or subtract one cell (2 bytes). All cursor arithmetic is in linear offsets, so moving across a wrapped line works.

### Clear screen

`clear_screen(alias)` blanks everything **below** the header row, puts the cursor at row 1 and prints the prompt (`<alias>> `). It runs for the `clear` command and for **Ctrl+L**. (`clear_screen_full()` blanks all rows and is used at start-up.)

### Header and footer

- **Top bar (row 0):** `CURRENT DIRECTORY : <name>` followed by `| Up Time: <n>s`. The directory name is the *last component* of the current path (`/` for root) and is refreshed after every `cd` and after start-up. The clock field is redrawn by the timer interrupt.
- **Bottom bar:** `ZEOS Operating System !!`.

---

## 8. Basic keyboard input and line editing

**Status:** done. **Files:** `kernel/drivers/keyboard.c`.

### Scancode handling

`keyboard_callback()` runs on every IRQ1. It reads **one byte** from data port `0x60` (an earlier version read the port twice per interrupt; that was reduced to a single read).

- **Make/break:** scancode set 1: a byte with bit 7 set is a key *release*. Releases are only used to clear Shift (`0x2A`, `0x36`) and Ctrl (`0x1D`).
- **Layout tables:** `sc_ascii_non_shift[]` and `sc_ascii_shift[]` map a scancode to an ASCII character for keys up to scancode 57 (US layout).
- **Shift / Caps Lock:** a `shift_down` flag, set on Shift press, cleared on release, and toggled by Caps Lock (`0x3A`).
- **Extended keys:** the byte `0xE0` is a prefix. The driver remembers it (`extended_key`), and interprets the *next* byte as an arrow/Delete code, ignoring the release form (`code | 0x80`).

### The line editor

The driver keeps a 100-byte `key_buffer` and two state variables: `buff_pointer` (cursor position inside the buffer) and `max_length` (number of characters). The screen is kept in step with the buffer after every edit:

- **Typing** (`insert_char`): inserts *at the cursor*, shifting the rest of the line right, prints the character, then reprints the tail and moves the screen cursor back. So typing in the middle of a line inserts rather than overwrites. Input is capped at 99 characters.
- **Backspace** (`delete_before_cursor`): removes the character before the cursor, shifts the tail left, blanks the leftover cell.
- **Delete** (`manage_delete`): removes the character under the cursor and shifts the tail left.
- **← / →** (`manage_left`/`manage_right`): move within `[0, max_length]` only.
- **Ctrl+A / Ctrl+E:** jump to the start / end of the line.
- **Ctrl+L:** clear the screen and reset the buffer.
- **Enter:** first moves the cursor to the end of the line (so the newline is printed below the whole command, even if the cursor was mid-line), copies the line into `kb_c` for history, calls `user_input()` (the shell), then clears the buffer.
- **↑ (Up):** if the line is empty, recalls the **last** command (`kb_c`) into the buffer and prints it. Only one entry of history is kept.
- **↓ (Down):** currently just prints the text `DOWN` (a placeholder).

All commands are executed **inside the keyboard interrupt handler**, so a long-running command blocks the CPU until it returns (see [Shell](#15-the-shell-and-its-commands)).

---

## 9. Higher-half kernel

**Status:** done. **Files:** `conf/linker.ld`, `boot/boot.asm`.

A higher-half kernel is linked to run at a high virtual address (3 GB and up), leaving the low 3 GB for future user programs.

### Linking

`conf/linker.ld` sets `. = 0xC0100000`, so all symbols get virtual addresses at 3 GB + 1 MB. Each output section (`.text`, `.rodata`, `.data`, `.bss`) is aligned to 4 KB and declared with `AT(ADDR(section) - 0xC0000000)`: its **load address** (LMA) is the same location minus 3 GB, i.e. physical 1 MB. The script also exports `kernel_virtual_start/end` and `kernel_physical_start/end`, which `boot.asm` and `kernel_main` use to report the kernel's footprint.

### The bootstrap problem and its solution

GRUB starts the kernel with paging off, at *physical* addresses, but the code is linked for *virtual* ones. `boot.asm`'s `loader` therefore:

1. accesses its own variables through `symbol - KERNEL_VIRTUAL_BASE`, (physical addresses) while paging is off;
2. builds a page table `kernel_pt` that maps virtual `0xC0000000 + n` → physical `n`, and a page directory `kernel_pdt` with:
   - entry 0 = a **4 MB identity page** (`KERNEL_PDT_ID_MAP`), so the very next instruction after enabling paging is still valid (the CPU is executing at a low physical address at that moment), and
   - entry 768 (`0xC0000000 >> 22`) = the page table above;
3. loads `CR3`, sets `CR4.PSE` (4 MB pages), sets `CR0.PG` (paging on);
4. jumps to `higher_half_loader` (a virtual address above 3 GB, absolute jump through a register);
5. writes `0` into directory entry 0 and runs `invlpg [0]`, **removing the identity mapping** so low memory is no longer accessible at virtual 0 (a null-pointer dereference now page-faults);
6. sets `ESP` to `kernel_stack_lowest_address + KERNEL_STACK_SIZE` and calls `kernel_main` with the boot data pushed as arguments (Multiboot info pointer made virtual, kernel address ranges, page directory/table addresses).

The page table now maps **the first 4 MB of physical memory** (1024 entries). Previously it only mapped up to the end of the kernel image, which made GRUB modules (loaded straight after the kernel) unreachable. See [InitRD](#13-initrd).

### Layout

```
Virtual                     Physical
0xC0000000 ─────────────►   0x00000000   (BIOS, VGA at 0xB8000 → 0xC00B8000)
0xC0100000  kernel start ►  0x00100000   1 MB
   .text .rodata .data .bss (bss contains the 1 MB stack + heap region)
   initrd module (loaded by GRUB just after the kernel, page aligned)
0xC0400000  end of the mapped 4 MB
```

---

## 10. Paging

**Status:** boot-time paging is done and running; a more general page-management library exists but is **not wired in**.

**What is active:** the setup in `boot.asm` described above: one page directory, one page table, 4 KB pages, 4 MB mapped at `0xC0000000`. The page-fault handler in `isr.c` (vector 14) is active: it logs the faulting address and decodes the reason.

**What exists in the source but is not called** (`kernel/cpu/memory/paging.c`, `jpage.c`):

- `make_page_directory_entry` / `make_page_table_entry`: helpers that assemble entries from named options (page size, cache disabled, write-through, supervisor/user, read-only/read-write, present).
- A **physical page allocator** using a bitmap (`free_page_bitmap`, `BITMAP_SIZE` = 4096 words): `mark_free`, `mark_unavailable`, `allocate_physical_page`, and `initialize_page_allocator()` which walks the Multiboot memory map, marks available RAM free, and reserves the kernel's own pages.
- `initialize_page_directory()` and `print_page_directory()`.

The calls to these are commented out in `main.c`. Consequently there are currently no per-process address spaces, no dynamic mapping/unmapping, and RAM beyond 4 MB is not managed.

---

## 11. Heap

**Status:** done. **Files:** `kernel/cpu/memory/heap.c`, `kernel/include/cpu/memory/heap.h`.

Provides `malloc`, `free`, `calloc`, `realloc` and usage statistics.

### Where it lives

The heap occupies the **bottom of the 1 MB boot stack region** (`kernel_stack_lowest_address`, in `.bss`); the stack grows down from the top of the same region. The heap may grow up to `1 MB − 256 KB` (`MAIN_STACK_RESERVE`), leaving the top 256 KB for the main kernel stack. That gives the heap roughly 768 KB. Growth is done by moving a "break" pointer (`heap_brk`) upward, like `sbrk`.

### Algorithm: first-fit free list

- Every allocation is preceded by a 32-byte `heap_block_t` header: `magic` (`0xC0DEB10C`), payload `size`, `is_free`, `next`/`prev` links and padding.
- Blocks form one **address-ordered doubly linked list** covering the whole heap, so a block's neighbours in the list are also its neighbours in memory.
- **`malloc`:** rounds the size up to 8 bytes, scans for the first free block that fits (`heap_split` cuts it in two if the leftover can hold a header plus 16 bytes), otherwise extends the heap with a new block. Returns `NULL` when the limit is reached (it logs "Heap: out of memory").
- **`free`:** validates the pointer (range and magic; double frees are detected and logged, not acted on), marks the block free, **coalesces** with the next block and the previous block if they are free, and if the resulting block is at the very top of the heap, **lowers the break** to give the memory back.
- **`calloc`:** overflow-checked `count × size`, then zero-fills.
- **`realloc`:** shrinks in place (splitting), or allocates a new block, copies, and frees the old one. `realloc(NULL, n)` = `malloc(n)`, `realloc(p, 0)` = `free(p)`.
- **`heap_get_stats`:** walks the list and reports used/free bytes, header overhead, block counts, largest free block, start/end/limit.

### Interrupt safety

Threads and interrupt handlers may call the allocator at any time, so each public entry point saves `EFLAGS`, executes `cli`, does its work, and restores the saved flags (`heap_lock` / `heap_unlock`). Saving the flags instead of blindly re-enabling interrupts makes the lock nest correctly (e.g. `realloc` calling `malloc`).

### Verification

- The allocator source was compiled as a host program and put through **200,000 random `malloc` / `realloc` / `free` operations** with per-block content checks: no corruption, all pointers 8-byte aligned, and after freeing everything `heap_used_bytes()` was 0 and the break returned to the heap start. This test found and led to fixing a fragmentation bug in `realloc`'s split path.
- Running in QEMU: the VFS, thread creation and `mem` command all run on top of it.

### Users of the heap

The VFS (nodes, file data, path components, `ls` entries), thread structures and thread stacks (16 KB each).

---

## 12. Multitasking (kernel threads)

**Status:** done for **kernel threads**, preemptive round-robin. **Files:** `kernel/cpu/thread.c`, `kernel/include/cpu/thread.h`, `boot/thread.asm`, hook in `kernel/cpu/isr.c`, demo commands in `kernel/shell/shell.c`.

### Data structures

```c
typedef struct thread {
    uint32_t id;  char name[16];
    thread_state_t state;        // RUNNING / READY / SLEEPING / ZOMBIE
    uint32_t esp;                // saved stack pointer while switched out
    uint8_t *stack; uint32_t stack_size;   // malloc'd 16 KB (NULL for main)
    uint32_t wake_tick;          // for SLEEPING
    uint32_t run_ticks;          // timer ticks spent running
    uint32_t work;               // demo counter
    struct thread *next;
} thread_t;
```

All threads are on a singly linked list whose head is the static `main_thread` (id 0), the boot context. It is the **idle thread**: it never sleeps or exits, and `kernel_main` ends in a `hlt` loop, so the scheduler always has something to run. At most 16 threads (`MAX_THREADS`) exist at once.

### Context switch (`boot/thread.asm`)

`switch_context(uint32_t *old_esp, uint32_t new_esp)` pushes the callee-saved registers (`ebp`, `ebx`, `esi`, `edi`), stores `ESP` into `*old_esp`, loads `new_esp`, pops the same four registers from the new stack and `ret`s. Only callee-saved registers need saving because the switch happens as an ordinary function call (caller-saved registers are already dead per the C ABI).

### Creating a thread

`thread_create(name, fn, arg)` allocates a `thread_t` and a 16 KB stack, then **fabricates the stack frame that `switch_context` will "return" into**: six words holding `edi=0`, `esi=arg`, `ebx=fn`, `ebp=0`, the return address `thread_entry_asm`, and padding. The first time the scheduler switches to the new thread, `switch_context` pops those into the registers and `ret`s into `thread_entry_asm`, which:

1. executes `sti` (a new thread may be started from *inside* the timer interrupt, where interrupts are off),
2. pushes `arg` and calls `fn`,
3. if `fn` returns, calls `thread_exit`.

### Scheduling

- **Preemption:** the timer interrupt handler calls `scheduler_tick()` after the EOI. It increments the running thread's `run_ticks` and calls `schedule()`. Since the interrupt frame lives on the *interrupted thread's own stack*, switching stacks inside the handler is safe. When that thread is resumed later, it returns from `switch_context` back through the handler, `iret`s, and continues exactly where it was interrupted.
- **`schedule()`:** round-robin. Starting after the current thread it looks for the first `READY` thread (a `SLEEPING` thread whose `wake_tick` has passed becomes `READY`), wraps around, and falls back to `main`.
- **Cooperative API:** `thread_yield()` (masks interrupts and calls `schedule()`), `thread_sleep(ticks)` (sets `wake_tick = tick + ticks`, state `SLEEPING`, yields).
- **Exit and kill:** `thread_exit()` and `thread_kill(id)` mark the thread `ZOMBIE`. A zombie is never selected again. **Reaping** (unlinking, freeing the stack and struct) is done by `reap_zombies()` at the start of a later `schedule()`, and never for the *currently running* thread, because that would free the stack being executed on. `thread_kill(0)` is refused.
- **Critical sections** use `pushf; cli … popf`, the same nesting-safe pattern as the heap.

### Shell integration

| Command | Effect |
|---|---|
| `spawn counter` | Thread that increments a counter in a busy loop. Proves preemption of a thread that never yields. |
| `spawn ticker` | Increments, then `thread_sleep(25)` (~0.5 s) in a loop. Proves sleep/wake-up. |
| `spawn burst` | Counts to 3,000,000, then returns. Proves normal exit and reaping. |
| `ps` | Lists id, name, state, ticks run and work counter of every thread. |
| `kill <id>` | Stops a thread. |

### Verified (QEMU direct boot)

`spawn counter` accumulated hundreds of millions of counts while the shell stayed responsive; `ticker` showed `sleeping` and a small work count; `burst` and a killed `counter` disappeared from `ps`. The stress/leak scenario (many spawn/kill cycles, hitting the 16-thread limit) was **not** run.

### Limits

- The **shell is not a thread**: commands execute inside the keyboard interrupt handler, on whichever thread was interrupted, with interrupts off. A slow command blocks scheduling for its duration, and the `delay` command still freezes the system.
- Thread stacks are 16 KB partly because interrupts and shell commands run on them too.
- Only the heap is thread-safe. The VFS and the screen are not.
- No priorities, no blocking primitives (mutexes, semaphores), no user-mode processes.

---

## 13. InitRD

**Status:** done. **Files:** `kernel/fs/initrd.c`, `kernel/include/fs/initrd.h`, `scripts/make_initrd.py`, `initrd/`, `Makefile`, `boot/boot.asm`.

An initial ramdisk is a small file-set packed into one image and loaded into RAM by the bootloader, so the kernel has files available without a disk driver.

### Image format (little-endian)

```
uint32   file_count
file_count × header (76 bytes):
    uint8   magic        = 0xBF
    char    name[64]     path relative to the source directory, '/'-separated, NUL-padded
    3 bytes padding
    uint32  offset       absolute offset of the file data within the image
    uint32  length
... file data ...
```

This is the format of the `initrd.img` that was already in the repository (the header layout matches the compiler's natural struct alignment: 1 + 64 + 3 padding + 4 + 4 = 76 bytes). The kernel asserts the struct size at compile time (`_Static_assert(sizeof(initrd_header_t) == 76)`).

### Building the image

`scripts/make_initrd.py <dir> <out>` walks the directory, checks every relative path fits in 63 characters, writes the header table, then the data. The Makefile rule `initrd.img: $(shell find initrd -type f)` regenerates the image when anything under `initrd/` changes, and `zeos.iso` depends on it. Contents shipped today: `t1`, `t2`, `docs/about.txt`.

### Getting the image into memory

`conf/menu.lst` has `module /module/initrd.img`, so GRUB loads it into RAM (page-aligned, directly after the kernel) and lists it in the Multiboot info (`mods_count`, `mods_addr`). Because the boot code now maps the first 4 MB (see [Higher-half kernel](#9-higher-half-kernel)), the module is reachable at `0xC0000000 + mod_start`. `initrd_load()` refuses modules that end beyond the mapped 4 MB, so an oversized image is rejected cleanly rather than crashing.

### Loading (`initrd_load`, called after `init_shell()`)

1. Check `MULTIBOOT_INFO_MODS` and `mods_count > 0`; take the first module.
2. Validate: image at least 4 bytes, and the header table fits inside the image.
3. Create `/initrd` in the VFS.
4. For every entry: check `magic`, that the name is non-empty and NUL-terminated, that `offset + length` lies inside the image, and that `length ≤ MAX_FILE_SIZE` (4096). Bad entries are logged and skipped, never trusted.
5. `extract_file()` builds `/initrd/<name>`, creates any intermediate directories (`make_parent_dirs`), creates the file, opens it, writes the data with `vfs_write`, and closes it.

The files are **copied** from the module into the VFS heap, so the VFS owns its copies and the module memory is not referenced afterward. Extraction is one-way; changes made in the shell are not written back.

### Inspecting it

The `initrd` shell command prints the module's physical address range, size, number of files, how many were extracted, the mount point, and a table of each entry's name, offset, size and status (`loaded`, `bad header`, `out of range`, `too large`, `failed`). `initrd.c` records this per-entry outcome in a table of up to 64 entries while loading. Without a module it prints `initrd: no image loaded (no GRUB module)`.

### Verified (QEMU direct boot with `-initrd`)

`ls /initrd`, `ls /initrd/docs`, `cat /initrd/t1` and `cat /initrd/docs/about.txt` return the expected content; a deliberately oversized (5000-byte) file was reported as `too large` while the others loaded; without a module the loader logs and the command reports no image. Booting through the GRUB ISO was not tested.

---

## 14. VFS

**Status:** done: an in-memory virtual file system. **Files:** `kernel/fs/fs_new.c`, `kernel/include/fs/fs.h`. (`VFS_DOCUMENTATION.md` and `kernel/fs/fs.md` hold older prose notes.)

### Model

The VFS is a **tree of `vfs_node_t`** allocated on the heap. A node is either a directory or a regular file (a tagged union):

- `vfs_directory_t`: `name[64]`, `children[32]`, `child_count`, `permissions`, `creation_time`, `parent`
- `vfs_file_t`: `name[64]`, `size`, `allocated_size`, `data` (heap buffer), `permissions`, `creation_time`, `modification_time`, `parent`

Global state (`vfs_state_t`): the `root`, the `current_directory` node, the current path string, and a table of **16 open file descriptors** (`node`, `position`, `flags`, `is_open`). A `vfs_operations_t` table of function pointers is filled in `vfs_init()` for a future pluggable back end.

Limits: file name 64, path 256, **file size 4096 bytes**, 32 children per directory, 16 open files.

### Operations

| Function | Behaviour |
|---|---|
| `vfs_init` | Creates the root `/` and sets the current directory and path. |
| `vfs_split_path` | Splits a path on `/` into heap-allocated components. |
| `vfs_find_node` | Resolves absolute (`/a/b`) and relative paths, handling `.` and `..`. Returns `NULL` if any component is missing. |
| `vfs_create_file` / `vfs_create_directory` | Resolve the parent, fail with `EXISTS` on a duplicate, create the node and link it into the parent. |
| `vfs_delete_file` / `vfs_delete_directory` | Unlink the node from its parent and free it. Directories must be empty, and a directory that is (or contains) the current directory cannot be removed. |
| `vfs_open` | Finds the node, creates it when `O_CREAT` is given, allocates a descriptor, honours `O_TRUNC` / `O_APPEND`. |
| `vfs_read` / `vfs_write` | Byte-position based. Writes grow the buffer with `realloc` up to `MAX_FILE_SIZE`, and update `size` and the modification time. |
| `vfs_close` | Releases the descriptor. |
| `vfs_list_directory` | Returns freshly allocated name strings (the caller frees them). |
| `vfs_change_directory` | Changes the current directory **and rebuilds the absolute path by walking parent pointers up to the root**, so `pwd` always prints a full path from `/`. |

### Memory management

Since the heap gained `free`, the VFS releases everything it allocates: `vfs_destroy_node` frees a file's data and then the node (recursively for directories), path components are freed after every lookup, and the shell frees `ls` entries after printing. File writes use `realloc` to grow buffers.

### Shell integration

`ls [path]`, `mkdir <path>`, `touch <file>`, `rm <path>`, `cd <path>`, `pwd`, `cat <file>`, `echo <text> [> file]`. Implemented as the `vfs_shell_*` helpers, which translate error codes into readable messages.

- **`echo`** is parsed from the original, case-preserved command line. Double quotes group words and are stripped from the output, a `>` inside quotes is literal, and an unquoted `>` redirects to a file (`echo "Hello   World" > out.txt`).
- **`pwd`** prints the full path from `/`.

### Defaults created at start-up

`/home`, `/tmp`, `/usr`, `/etc`, `/readme.txt` (sample text), and `/initrd` (see above).

### Limits and notes

- Contents live only in RAM; nothing persists across reboot.
- File permissions are stored on nodes but **not enforced** anywhere.
- Timestamps come from `rdtsc` and are not calendar times.
- No locking, so it is not safe to use from several threads at once (it is currently only used from the shell, i.e. interrupt context).

---

## 15. The shell and its commands

**Files:** `kernel/shell/shell.c`.

`user_input()` is called by the keyboard driver on Enter. It saves a copy of the raw line (for `echo`), collapses repeated spaces, lowercases the line, splits it into words, and dispatches: VFS commands and `spawn`/`kill`/`echo` are matched on the first word, everything else goes to `manage_input()` for single-word commands. After the command runs, the prompt (`<alias>> `) is printed once (commands that print their own prompt, like `clear`, suppress the duplicate).

| Command | Description |
|---|---|
| `help` | Lists all commands |
| `ls [path]`, `mkdir <path>`, `touch <file>`, `rm <path>`, `cd <path>`, `pwd`, `cat <file>`, `echo <text> [> file]` | File system commands (see [VFS](#14-vfs)) |
| `mem` | Kernel heap usage (in use / free / headers / heap size / room to grow / largest free block / block counts) and stack usage of the running thread |
| `ps`, `spawn <counter\|ticker\|burst>`, `kill <id>` | Threads (see [Multitasking](#12-multitasking-kernel-threads)) |
| `initrd` | Loaded initrd image details (see [InitRD](#13-initrd)) |
| `alias` | Sets the prompt name to the fixed string `dedd` (takes no argument) |
| `time` | Prints the raw time-stamp counter high word |
| `delay` | Busy-waits; blocks the whole system while it runs |
| `clear` | Clears the screen (also Ctrl+L) |
| `end` | Prints a message and halts the CPU (`hlt`) |

The welcome banner is drawn at boot, then replaced by the header bar and the prompt.

---

## 16. Build system and ISO creation

**Files:** `Makefile`, `conf/`, `stage2_eltorito`, `scripts/`.

### Toolchain selection

The Makefile picks the compiler from the host OS:

1. If `i686-elf-gcc` is found (in `/opt/homebrew/bin`, `/usr/local/bin` or on `PATH`) it is used on any OS. This is the recommended freestanding cross compiler and is required on macOS.
2. Otherwise on Linux the host `gcc` is used in 32-bit mode: the link step gets `-m32 -no-pie`, and the compile step gets `-U__linux__` so that the "You are not using a cross-compiler" guard in `kernel/main.c` does not trigger. Without 32-bit `libgcc` installed, `-lgcc` is omitted.
3. Any other OS without a cross compiler stops with a message explaining how to install one.

Assembly uses `nasm -felf32`. C is compiled with `-ffreestanding -m32 -march=i386 -fno-pie -fno-exceptions -O2 -Wall -Wextra`.

### Targets

- `make`: builds `zeos.iso` (kernel → `zeos.bin`, packs `initrd.img`, then creates the ISO).
- `make run`: boots the ISO in `qemu-system-i386` (4 GB RAM), serial output to `log/log.txt`.
- `make debug`: same with QEMU `-d guest_errors -D debug.txt`.
- `make clean`: removes objects, `zeos.bin`, the ISO, `isodir/`, `log/`.

### How the ISO is made

1. Compile all `boot/*.asm` and every `.c` under `kernel/…` into objects.
2. Link with `conf/linker.ld` (`-nostdlib`) into `zeos.bin`.
3. Build `isodir/` with the GRUB legacy layout:
   - `isodir/boot/zeos.bin`: the kernel
   - `isodir/boot/grub/stage2_eltorito`: GRUB's El Torito boot image
   - `isodir/boot/grub/menu.lst`: from `conf/menu.lst` (`kernel /boot/zeos.bin`, `module /module/initrd.img`)
   - `isodir/module/initrd.img`: the ramdisk
4. Run `mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o zeos.iso isodir`. `-no-emul-boot` and `-boot-load-size 4` describe the El Torito boot image, and `-boot-info-table` patches it with the disc layout so stage2 can find its files.

Requirements: `nasm`, a C compiler (cross recommended), `mkisofs` (`genisoimage` on Debian/Ubuntu), `python3` (initrd packer), `qemu-system-i386`.

---

## 17. Not implemented yet

From the README's unchecked items:

- **User mode.** Needs a TSS, ring-3 GDT descriptors, per-process page directories and a syscall interface.
- **PCI.** No bus enumeration.
- **Connectivity** (basic algorithms, protocols). No network driver or stack.
- **Programs**: alarm, mini REPL-interpreter, calculator.

Related gaps worth knowing about: the shell is not its own thread, the page allocator code exists but is unused, there is no persistent storage (disk driver), and the VFS has no permissions enforcement or locking.

---

## 18. Known issues and README corrections

### README items that are out of date

- `Implement delete function for deleting backways {Not Implementing}`: **Backspace and the Delete key both work now.**
- `Implement C-A and C-E (endofline) {C-E Not working}`: **Ctrl+A and Ctrl+E both work** (tested).
- `Heap → malloc`: the heap now also has `free`, `calloc`, `realloc` and statistics.
- `Paging` is checked, but only the boot-time mapping is active (see [section 10](#10-paging)).

### Bugs and risks found while documenting

- **Segment registers are zeroed after the first interrupt.** In `common_interrupt_handler`, `ds` is never saved (`mov eax, cr2` overwrites it), and the restore path loads `DS/ES/FS/GS` from the saved CR2 slot. Inspecting a running kernel in QEMU shows `DS = ES = FS = GS = 0` after start-up. QEMU tolerates this, but real hardware treats a data access through a null selector as a general-protection fault, so this needs fixing before trusting the kernel on real machines. The fix is to push `ds` properly and restore it.
- **Wrong error-code handling for some exceptions.** The stubs for vectors 8 and 10–13 push a dummy error code, but the CPU already pushes a real one for those, so the frame layout is off by one word for them. Only the page-fault stub (14) is written correctly.
- **Only vectors 14, 32 and 33 are handled.** Any other exception (including a general-protection fault) is only logged; the handler then returns, so for a fault the CPU re-executes the same instruction and typically loops forever instead of reporting or recovering.
- **Uptime field** is derived from the high word of `rdtsc`, not real time.
- **`time` and file timestamps** use the same raw counter.
- **Heap is capped at ~768 KB** and lives inside the kernel image's `.bss`, unconnected to the paging code.
- **Longest command line is 99 characters**, and history is a single entry.
- **Commands run in interrupt context.** `delay` (and any slow command) freezes scheduling and input until it returns.
- **File size limit** is 4096 bytes; larger initrd files are skipped.
- **The Down arrow** prints `DOWN` into the line without touching the buffer.
