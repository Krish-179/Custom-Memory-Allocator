#ifndef ALLOCATOR_H
#define ALLOCATOR_H
	
#include <stddef.h>

void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_calloc(size_t n_elements, size_t size);
void *my_realloc(void *ptr, size_t size);
void print_blocks(void);

#endif
