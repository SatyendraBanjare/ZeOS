#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>
#include <stddef.h>

#define THREAD_STACK_SIZE 16384     /* interrupts and shell commands run on this stack too */
#define THREAD_NAME_LEN   16
#define MAX_THREADS       16

typedef enum {
    THREAD_RUNNING,
    THREAD_READY,
    THREAD_SLEEPING,
    THREAD_ZOMBIE
} thread_state_t;

typedef struct thread {
    uint32_t id;
    char name[THREAD_NAME_LEN];
    thread_state_t state;
    uint32_t esp;                   /* saved stack pointer while not running */
    uint8_t *stack;                 /* base of the malloc'd stack (NULL for main) */
    uint32_t stack_size;
    uint32_t wake_tick;             /* THREAD_SLEEPING: wake when tick >= this */
    uint32_t run_ticks;             /* timer ticks spent running */
    uint32_t work;                  /* free counter for thread bodies to bump */
    struct thread *next;
} thread_t;

typedef void (*thread_fn)(void *arg);

void threads_init(void);                                    /* adopt the boot context as thread 0 */
int  thread_create(const char *name, thread_fn fn, void *arg);   /* thread id, or -1 */
void thread_yield(void);
void thread_sleep(uint32_t ticks);                          /* timer ticks (50 Hz) */
void thread_exit(void);
int  thread_kill(uint32_t id);                              /* 0 ok, -1 not found, -2 not allowed */
void scheduler_tick(void);                                  /* call from the timer IRQ after EOI */

thread_t *thread_current(void);
thread_t *thread_first(void);                               /* list head, for ps */
uint32_t  thread_count(void);
const char *thread_state_name(thread_state_t s);
void thread_stack_bounds(uint32_t *base, uint32_t *size);   /* of the running thread */

#endif
