#ifndef FIRST_FIT_H
#define FIRST_FIT_H

#include <stdlib.h>

typedef struct MemoryBlock {
    size_t size;
    int is_free;
    struct MemoryBlock* next;
    struct MemoryBlock* prev;
} MemoryBlock;


void* initialize_memory_pool();
void* free_memory_pool();
void* cool_malloc(size_t size);
void* cool_free(void* ptr);
void* cool_realloc(void* ptr, size_t size);

#endif
