#include "../../include/cpu/memory/heap.h"

/*
 * Kernel heap: a first-fit free-list allocator.
 *
 * The heap lives at the bottom of the kernel stack area (the stack grows down
 * from the top of the same region) and grows upwards on demand. Every block is
 * preceded by a header; blocks are kept in one address-ordered doubly linked
 * list that covers [heap_start, heap_brk). Freed blocks are coalesced with
 * their neighbours, and a free block at the top of the heap shrinks it again.
 */

#define HEAP_ALIGN      8
#define HEAP_MAGIC      0xC0DEB10C
#define HEAP_MIN_SPLIT  16                 /* smallest useful payload */
#define HEAP_REGION_SIZE 0x100000          /* heap + main kernel stack share 1 MB */
#define MAIN_STACK_RESERVE 0x40000         /* top 256 KB is left for the main stack */

typedef struct heap_block {
	uint32_t magic;
	uint32_t size;                          /* payload size in bytes */
	uint32_t is_free;
	struct heap_block *next;
	struct heap_block *prev;
	uint32_t pad[3];                        /* header = 32 bytes, keeps payload aligned */
} heap_block_t;

#define HEADER_SIZE ((uint32_t)sizeof(heap_block_t))

/* The heap can be entered from any thread and from interrupt handlers, so every
 * public entry point masks interrupts. The saved flags make this nest safely. */
static inline uint32_t heap_lock(void) {
	uint32_t flags;
	asm volatile("pushf; pop %0; cli" : "=r"(flags) : : "memory");
	return flags;
}

static inline void heap_unlock(uint32_t flags) {
	asm volatile("push %0; popf" : : "r"(flags) : "memory", "cc");
}

static uint8_t *heap_start = NULL;
static uint8_t *heap_brk = NULL;            /* first byte past the last block */
static heap_block_t *heap_head = NULL;
static heap_block_t *heap_tail = NULL;

static uintptr_t align_up(uintptr_t n) {
	return (n + (HEAP_ALIGN - 1)) & ~(uintptr_t)(HEAP_ALIGN - 1);
}

static void heap_init(void) {
	heap_start = (uint8_t *)align_up((uintptr_t)&kernel_stack_lowest_address);
	heap_brk = heap_start;
}

/* Grow the heap by one block of `size` payload bytes; NULL if out of memory. */
static heap_block_t *heap_extend(uint32_t size) {
	uint32_t total = HEADER_SIZE + size;
	uint8_t *limit = heap_start + (HEAP_REGION_SIZE - MAIN_STACK_RESERVE);

	if (heap_brk + total > limit || heap_brk + total < heap_brk) {
		print_log("Heap: out of memory");
		return NULL;
	}

	heap_block_t *b = (heap_block_t *)heap_brk;
	heap_brk += total;

	b->magic = HEAP_MAGIC;
	b->size = size;
	b->is_free = 0;
	b->next = NULL;
	b->prev = heap_tail;
	if (heap_tail) heap_tail->next = b; else heap_head = b;
	heap_tail = b;
	return b;
}

/* Split `b` so it holds exactly `size` bytes, if the remainder is worth a block. */
static void heap_split(heap_block_t *b, uint32_t size) {
	if (b->size < size + HEADER_SIZE + HEAP_MIN_SPLIT) return;

	heap_block_t *rest = (heap_block_t *)((uint8_t *)b + HEADER_SIZE + size);
	rest->magic = HEAP_MAGIC;
	rest->size = b->size - size - HEADER_SIZE;
	rest->is_free = 1;
	rest->prev = b;
	rest->next = b->next;
	if (b->next) b->next->prev = rest; else heap_tail = rest;
	b->next = rest;
	b->size = size;

	/* the remainder may now sit next to another free block: merge them */
	heap_block_t *n = rest->next;
	if (n && n->is_free) {
		rest->size += HEADER_SIZE + n->size;
		rest->next = n->next;
		if (n->next) n->next->prev = rest; else heap_tail = rest;
		n->magic = 0;
	}
}

static void *malloc_locked(uint32_t bytes) {
	if (!heap_start) heap_init();
	if (bytes == 0) bytes = 1;
	bytes = (uint32_t)align_up(bytes);

	for (heap_block_t *b = heap_head; b; b = b->next) {
		if (b->is_free && b->size >= bytes) {
			heap_split(b, bytes);
			b->is_free = 0;
			return (uint8_t *)b + HEADER_SIZE;
		}
	}

	heap_block_t *b = heap_extend(bytes);
	return b ? (void *)((uint8_t *)b + HEADER_SIZE) : NULL;
}

static void free_locked(void *ptr) {
	if (!ptr) return;

	heap_block_t *b = (heap_block_t *)((uint8_t *)ptr - HEADER_SIZE);
	if ((uint8_t *)b < heap_start || (uint8_t *)ptr >= heap_brk ||
	    b->magic != HEAP_MAGIC) {
		print_log("Heap: free() of invalid pointer");
		return;
	}
	if (b->is_free) {
		print_log("Heap: double free");
		return;
	}
	b->is_free = 1;

	/* merge with the next block */
	if (b->next && b->next->is_free) {
		heap_block_t *n = b->next;
		b->size += HEADER_SIZE + n->size;
		b->next = n->next;
		if (n->next) n->next->prev = b; else heap_tail = b;
		n->magic = 0;
	}
	/* merge with the previous block */
	if (b->prev && b->prev->is_free) {
		heap_block_t *p = b->prev;
		p->size += HEADER_SIZE + b->size;
		p->next = b->next;
		if (b->next) b->next->prev = p; else heap_tail = p;
		b->magic = 0;
		b = p;
	}
	/* a free block at the top gives its memory back */
	if (b == heap_tail) {
		heap_tail = b->prev;
		if (heap_tail) heap_tail->next = NULL; else heap_head = NULL;
		heap_brk = (uint8_t *)b;
		b->magic = 0;
	}
}

void *malloc(uint32_t bytes) {
	uint32_t f = heap_lock();
	void *p = malloc_locked(bytes);
	heap_unlock(f);
	return p;
}

void free(void *ptr) {
	uint32_t f = heap_lock();
	free_locked(ptr);
	heap_unlock(f);
}

void *calloc(uint32_t count, uint32_t size) {
	if (size != 0 && count > 0xFFFFFFFFu / size) return NULL;
	uint32_t total = count * size;
	uint8_t *p = malloc(total);
	if (p) {
		for (uint32_t i = 0; i < total; i++) p[i] = 0;
	}
	return p;
}

static void *realloc_locked(void *ptr, uint32_t bytes) {
	if (!ptr) return malloc(bytes);
	if (bytes == 0) { free(ptr); return NULL; }

	heap_block_t *b = (heap_block_t *)((uint8_t *)ptr - HEADER_SIZE);
	if (b->magic != HEAP_MAGIC || b->is_free) {
		print_log("Heap: realloc() of invalid pointer");
		return NULL;
	}

	uint32_t want = (uint32_t)align_up(bytes);
	if (want <= b->size) {
		heap_split(b, want);
		return ptr;
	}

	uint8_t *n = malloc(bytes);
	if (!n) return NULL;
	uint8_t *src = ptr;
	for (uint32_t i = 0; i < b->size; i++) n[i] = src[i];
	free(ptr);
	return n;
}

void *realloc(void *ptr, uint32_t bytes) {
	uint32_t f = heap_lock();
	void *r = realloc_locked(ptr, bytes);
	heap_unlock(f);
	return r;
}

uint32_t heap_used_bytes(void) {
	uint32_t used = 0;
	for (heap_block_t *b = heap_head; b; b = b->next) {
		if (!b->is_free) used += b->size;
	}
	return used;
}

void heap_get_stats(heap_stats_t *st) {
	uint32_t f = heap_lock();
	if (!heap_start) heap_init();

	st->heap_start = (uint32_t)(uintptr_t)heap_start;
	st->heap_end = (uint32_t)(uintptr_t)heap_brk;
	st->heap_limit = (uint32_t)(uintptr_t)(heap_start + (HEAP_REGION_SIZE - MAIN_STACK_RESERVE));
	st->used_bytes = 0;
	st->free_bytes = 0;
	st->overhead_bytes = 0;
	st->used_blocks = 0;
	st->free_blocks = 0;
	st->largest_free = 0;

	for (heap_block_t *b = heap_head; b; b = b->next) {
		st->overhead_bytes += HEADER_SIZE;
		if (b->is_free) {
			st->free_bytes += b->size;
			st->free_blocks++;
			if (b->size > st->largest_free) st->largest_free = b->size;
		} else {
			st->used_bytes += b->size;
			st->used_blocks++;
		}
	}
	heap_unlock(f);
}
