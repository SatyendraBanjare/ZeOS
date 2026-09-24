#include "../include/cpu/thread.h"
#include "../include/cpu/memory/heap.h"
#include "../include/cpu/state.h"
#include "../include/common/helper.h"

extern uint32_t tick;                       /* timer.c, 50 Hz */
extern void switch_context(uint32_t *old_esp, uint32_t new_esp);
extern void thread_entry_asm(void);

#define MAIN_STACK_TOP_OFFSET 0x100000      /* boot stack region size */
#define MAIN_STACK_RESERVE    0x40000       /* matches heap.c */

static thread_t main_thread;
static thread_t *head = NULL;               /* main thread is always the head */
static thread_t *current = NULL;
static uint32_t next_id = 1;
static uint32_t nthreads = 0;

static inline uint32_t irq_save(void) {
    uint32_t flags;
    asm volatile("pushf; pop %0; cli" : "=r"(flags) : : "memory");
    return flags;
}

static inline void irq_restore(uint32_t flags) {
    asm volatile("push %0; popf" : : "r"(flags) : "memory", "cc");
}

void threads_init(void) {
    main_thread.id = 0;
    const char *n = "main";
    int i;
    for (i = 0; n[i]; i++) main_thread.name[i] = n[i];
    main_thread.name[i] = '\0';
    main_thread.state = THREAD_RUNNING;
    main_thread.esp = 0;
    main_thread.stack = NULL;
    main_thread.stack_size = MAIN_STACK_RESERVE;
    main_thread.wake_tick = 0;
    main_thread.run_ticks = 0;
    main_thread.work = 0;
    main_thread.next = NULL;

    head = &main_thread;
    current = &main_thread;
    nthreads = 1;
}

/* Free threads that have exited or been killed. Never touches the running one,
 * whose stack we are standing on. */
static void reap_zombies(void) {
    thread_t *prev = head;
    thread_t *t = head->next;
    while (t) {
        thread_t *next = t->next;
        if (t->state == THREAD_ZOMBIE && t != current) {
            prev->next = next;
            free(t->stack);
            free(t);
            nthreads--;
        } else {
            prev = t;
        }
        t = next;
    }
}

static int runnable(thread_t *t) {
    if (t->state == THREAD_SLEEPING && (int32_t)(tick - t->wake_tick) >= 0) {
        t->state = THREAD_READY;
    }
    return t->state == THREAD_READY || t->state == THREAD_RUNNING;
}

/* Round robin: first runnable thread after the current one, wrapping around.
 * Must be called with interrupts disabled. */
static void schedule(void) {
    if (!current) return;

    thread_t *prev = current;
    reap_zombies();

    thread_t *next = NULL;
    thread_t *t = prev->next ? prev->next : head;
    for (uint32_t i = 0; i < nthreads; i++) {
        if (runnable(t)) { next = t; break; }
        t = t->next ? t->next : head;
    }
    if (!next) next = head;             /* main is the idle thread and never blocks */
    if (next == prev) {
        if (prev->state == THREAD_READY) prev->state = THREAD_RUNNING;
        return;
    }

    if (prev->state == THREAD_RUNNING) prev->state = THREAD_READY;
    next->state = THREAD_RUNNING;
    current = next;
    switch_context(&prev->esp, next->esp);
}

int thread_create(const char *name, thread_fn fn, void *arg) {
    if (!current || !fn) return -1;

    uint32_t flags = irq_save();
    if (nthreads >= MAX_THREADS) { irq_restore(flags); return -1; }

    thread_t *t = malloc(sizeof(thread_t));
    uint8_t *stack = malloc(THREAD_STACK_SIZE);
    if (!t || !stack) {
        free(t);
        free(stack);
        irq_restore(flags);
        return -1;
    }

    t->id = next_id++;
    int i;
    for (i = 0; name && name[i] && i < THREAD_NAME_LEN - 1; i++) t->name[i] = name[i];
    t->name[i] = '\0';
    t->stack = stack;
    t->stack_size = THREAD_STACK_SIZE;
    t->wake_tick = 0;
    t->run_ticks = 0;
    t->work = 0;
    t->state = THREAD_READY;

    /* Fake the frame switch_context expects: edi, esi, ebx, ebp, return address */
    uint32_t *sp = (uint32_t *)(stack + THREAD_STACK_SIZE);
    *--sp = 0;                          /* padding, keeps the frame off the very top */
    *--sp = (uint32_t)(uintptr_t)thread_entry_asm;
    *--sp = 0;                          /* ebp */
    *--sp = (uint32_t)(uintptr_t)fn;    /* ebx */
    *--sp = (uint32_t)(uintptr_t)arg;   /* esi */
    *--sp = 0;                          /* edi */
    t->esp = (uint32_t)(uintptr_t)sp;

    t->next = NULL;
    thread_t *tail = head;
    while (tail->next) tail = tail->next;
    tail->next = t;
    nthreads++;

    uint32_t id = t->id;
    irq_restore(flags);
    return (int)id;
}

void thread_yield(void) {
    uint32_t flags = irq_save();
    schedule();
    irq_restore(flags);
}

void thread_sleep(uint32_t ticks) {
    uint32_t flags = irq_save();
    current->wake_tick = tick + ticks;
    current->state = THREAD_SLEEPING;
    schedule();
    irq_restore(flags);
}

void thread_exit(void) {
    asm volatile("cli");
    current->state = THREAD_ZOMBIE;
    schedule();                         /* never returns: a zombie is never picked */
    for (;;) asm volatile("hlt");
}

int thread_kill(uint32_t id) {
    if (id == 0) return -2;             /* main is the idle thread */

    uint32_t flags = irq_save();
    for (thread_t *t = head->next; t; t = t->next) {
        if (t->id == id && t->state != THREAD_ZOMBIE) {
            t->state = THREAD_ZOMBIE;   /* reaped by the next schedule() */
            irq_restore(flags);
            return 0;
        }
    }
    irq_restore(flags);
    return -1;
}

/* Timer interrupt: account the tick and preempt. Interrupts are already off. */
void scheduler_tick(void) {
    if (!current) return;
    current->run_ticks++;
    schedule();
}

thread_t *thread_current(void) { return current; }
thread_t *thread_first(void) { return head; }
uint32_t thread_count(void) { return nthreads; }

const char *thread_state_name(thread_state_t s) {
    switch (s) {
        case THREAD_RUNNING:  return "running";
        case THREAD_READY:    return "ready";
        case THREAD_SLEEPING: return "sleeping";
        default:              return "zombie";
    }
}

void thread_stack_bounds(uint32_t *base, uint32_t *size) {
    if (current && current->stack) {
        *base = (uint32_t)(uintptr_t)current->stack;
        *size = current->stack_size;
    } else {
        *base = (uint32_t)(uintptr_t)&kernel_stack_lowest_address + MAIN_STACK_TOP_OFFSET - MAIN_STACK_RESERVE;
        *size = MAIN_STACK_RESERVE;
    }
}
