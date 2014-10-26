/*
 * mm_alloc.c
 *
 * Stub implementations of the mm_* routines. Remove this comment and provide
 * a summary of your allocator's design here.
 */

#include "mm_alloc.h"

#include <stdlib.h>

/* Your final implementation should comment out this macro. */
#define MM_USE_STUBS
typedef struct mem *mem_ptr;
struct mem {
	void *pointer;
	size_t size;
	struct mem *prev;
	struct mem *next;
};
void insert_before(mem_ptr elem, mem_ptr list) {
	elem->next = list;
	elem->prev = list->prev;
	list->prev->next = elem;
	list->prev = elem;
}
void insert_after(mem_ptr elem, mem_ptr list) {
	elem->prev = list;
	elem->next = list->next;
	list->next->prev = elem;
	list->next = elem;
}
struct mem free_list = {0}; //beginning of list of free memory
struct mem used_list = {0}; //end of list of used memory
void *free_mem = 0;

void* mm_malloc(size_t size)
{
#ifdef MM_USE_STUBS
	return calloc(1, size);
#else
	
	struct mem *new_mem;// = malloc(sizeof(mem));
	struct mem *free_mem = free_list;
	struct mem *used_mem = used_list;
	struct mem *mem_block = free_list;
	void *new_mem;
	if (free_mem == 0) {
		while (mem_block->next != 0) {
			if (mem_block->size >= sizeof(mem)) {
				mem_block->size -= sizeof(mem);
				free_mem = mem_block
				break;
			}
			mem_block = mem_block->next;
		}
	}
	while (free_mem->next != 0) {
		if (free_mem->size = size) {
			free_mem->prev->next = free_mem->next;
			insert_before(free_mem, used_mem);
			return free_mem->pointer;
		} else if (free_mem->size > size) {
			new_mem = free_mem;
			free_mem = 0;
			new_mem->pointer = free_mem->pointer;
			new_mem->size = size;
			insert_before(new_mem, used_mem);
			free_mem->size -= size;
			free_mem->pointer += size;
			return new_mem->pointer;
		}
	}
	new_mem = free_mem;
	free_mem = 0;
	new_mem->pointer = sbrk(size);
	new_mem->size = size;
	insert_before(new_mem, used_mem);
	return new_mem->pointer;
#endif
}

void* mm_realloc(void* ptr, size_t size)
{
#ifdef MM_USE_STUBS
	return realloc(ptr, size);
#else
	void *new_ptr = mm_malloc(size);
	memcpy(new_ptr, ptr, size);
	mm_free(ptr);
#endif
}

void mm_free(void* ptr)
{
#ifdef MM_USE_STUBS
	free(ptr);
#else
	bool found = false;
	struct mem *used_mem = used_list;
	while (used_mem->prev != 0) {
		used_mem = used_mem->prev;
		if (used_mem->pointer == ptr) {
			found = true;
			break;
		}
	}
	if (!found) {
		#error invalid pointer passed to free
	}
	struct mem *free_mem = free_list;
	found = false;
	while (free_mem->next != 0) {
		if (free_mem->pointer < ptr && ptr < free_mem->next->pointer) {
			insert_after(used_mem, free_mem);
			found = true;
			while (free_mem->pointer + free_mem->size >= free_mem->next->pointer) {
				free_mem->size += free_mem->next->size;
				free_mem->next = free_mem->next->next;
				free_mem->next->prev = free_mem;
			}
		}
	}
	if (!found) {
		free_mem->next = used_mem;
		used_mem->prev = free_mem;
		used_mem->next = 0;
	}
#endif
}
