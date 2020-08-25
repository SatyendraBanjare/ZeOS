#include "../../include/cpu/memory/heap.h"

void* lowest_free_memory = &kernel_stack_lowest_address;

void * zmalloc(uint32_t bytes){

	void* chunk_start = lowest_free_memory;
	lowest_free_memory += bytes;

	if (lowest_free_memory >= get_current_stack_pointer())
	{
		print_log("Memory > current stack accessed, Buffer Overflow !!");
		asm volatile("hlt");	
	}

	return chunk_start;
}

void * zmalloc_p(uint32_t bytes, uint32_t* phys){

	*phys = lowest_free_memory; 

	void* chunk_start = lowest_free_memory;
	lowest_free_memory += bytes;

	if (lowest_free_memory >= get_current_stack_pointer())
	{
		print_log("Memory > current stack accessed, Buffer Overflow !!");
		asm volatile("hlt");	
	}

	return chunk_start;
}