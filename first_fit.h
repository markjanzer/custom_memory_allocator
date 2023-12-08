#ifndef FIRST_FIT_H
#define FIRST_FIT_H

#include <stdlib.h>
#include <stddef.h>

typedef struct MemoryBlock {
    size_t size;
    int is_free;
    struct MemoryBlock* next;
    struct MemoryBlock* prev;
} MemoryBlock;

#define MEMORY_POOL_SIZE 1000

void initialize_memory_pool();
void* my_malloc(size_t size);
void* my_free(void* ptr);
void* my_realloc(void* ptr, size_t size);

#endif
