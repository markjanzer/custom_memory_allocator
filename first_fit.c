#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

#include "first_fit.h"

static int is_free(MemoryBlock* block);

unsigned char memory_pool[MEMORY_POOL_SIZE];
MemoryBlock* free_list = NULL;

void initialize_memory_pool() {
  free_list = (MemoryBlock*)memory_pool;
  free_list->size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  free_list->is_free = 1;
  free_list->next = NULL;
  free_list->prev = NULL;
}

void* my_malloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  MemoryBlock* current = free_list;
  while (current) {
    if (current->is_free && current->size >= size) {
      current->is_free = 0;
      current->size = size;

      if (current->size < size - sizeof(MemoryBlock)) {
        MemoryBlock* next_block = (MemoryBlock*)((char*)(current + 1) + size);
        next_block->size = current->size - sizeof(MemoryBlock);
        next_block->is_free = 1;
        next_block->next = NULL;
        next_block->prev = current;
        current->next = next_block;
      }

      return (void*)(current);
    }
    current = current->next;
  }

  return NULL;
}

// This doesn't work, it doesn't try to reloacate the memory
// Nor does it preserve the data.
void* my_realloc(void* ptr, size_t size) {
  if (ptr == NULL) {
    return NULL;
  }

  if (size == 0) {
    return NULL;
  }

  MemoryBlock* block = (MemoryBlock*)(ptr);
  block->size = size;

  return ptr;
}

void* my_free(void* ptr) {
  if (ptr == NULL) {
    return NULL;
  }

  MemoryBlock* block = (MemoryBlock*)(ptr);
  block->is_free = 1;

  printf("freeing, this is the size: %zu\n", block->size);

  while(is_free(block->prev)) {
    printf("moving back\n");
    block = block->prev;
  }

  while(is_free(block->next)) {
    printf("moving forward\n");
    printf("%zu + %zu + %zu\n", block->size, block->next->size, sizeof(MemoryBlock));
    printf("new size: %zu\n", block->size + block->next->size + sizeof(MemoryBlock));
    block->size = block->size + block->next->size + sizeof(MemoryBlock);
    block->next = block->next->next;
    printf("pointer: %p, size: %zu\n", block, block->size);
  }

  return NULL;
}

static int is_free(MemoryBlock* block) {
  return block && block->is_free;
}

