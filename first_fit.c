#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h> 

#include "first_fit.h"

MemoryBlock* combine_surrounding_space(MemoryBlock* block);
size_t space_between_blocks(MemoryBlock* a, MemoryBlock* b);
void* next_byte(MemoryBlock* block);
MemoryBlock* expand_back_to(MemoryBlock* block, void* new_start);
void combine_b_into_a(MemoryBlock* a, MemoryBlock* b);

unsigned char memory_pool[MEMORY_POOL_SIZE];
MemoryBlock* free_list = NULL;

void initialize_memory_pool() {
  free_list = (MemoryBlock*)memory_pool;
  free_list->size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  free_list->is_free = 1;
  free_list->next = NULL;
  free_list->prev = NULL;
}

void* cool_malloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  MemoryBlock* current = free_list;
  while (current) {
    printf("current->is_free: %d\n", current->is_free);
    if (current->is_free) {
      printf("current->size: %zu\n", current->size);
      current = combine_surrounding_space(current);

      if (current->size >= size) {
        current->is_free = 0;
        current->size = size;

        // If there is space for a memory block
        if (space_between_blocks(current, current->next) > sizeof(MemoryBlock)) {
          // Create a new free memory block

          // Refactor using this after tests pass
          // create_memory_block_after(block);
          MemoryBlock* next_block = (MemoryBlock*)((char*)(current + 1) + size);
          next_block->next = current->next;
          next_block->prev = current;
          next_block->size = space_between_blocks(next_block, next_block->next) - sizeof(MemoryBlock);
          next_block->is_free = 1;
          current->next = next_block;
        }

        return (void*)(current + 1);
      }
    }
    current = current->next;
  }

  return NULL;
}

void* cool_free(void* ptr) {
  if (ptr == NULL) {
    return NULL;
  }

  MemoryBlock* block = (MemoryBlock*)(ptr - 1);
  block->is_free = 1;

  return NULL;
}

void* cool_realloc(void* ptr, size_t size) {
  if (size == 0 || ptr == NULL) {
    return NULL;
  }

  size_t old_size = ((MemoryBlock*)(ptr))->size;
  MemoryBlock* moved_block = combine_surrounding_space(ptr);

  if (moved_block->size >= size) {
    moved_block->size = size;
    return (void*)(moved_block + 1);
  } else {
    void* new_ptr = cool_malloc(size);

    if (new_ptr == NULL) {
      return NULL;
    }

    memmove(moved_block, (void*)((MemoryBlock*)(new_ptr) - 1), old_size);
    return new_ptr;
  }
}

void* next_byte(MemoryBlock* block) {
  return (char*)(block + 1) + block->size + 1;
}

size_t space_between_blocks(MemoryBlock* a, MemoryBlock* b) {
  char* start_location = (char*)(next_byte(a));
  char* end_location;

  if (b == NULL) {
    end_location = (char*)free_list + MEMORY_POOL_SIZE - 1;
  } else {
    end_location = (char*)b;
  }

  return (size_t)(end_location - start_location);
}

size_t true_size(MemoryBlock* block) {
  return block->size + sizeof(MemoryBlock);
}

// expand_back_to
MemoryBlock* expand_back_to(MemoryBlock* block, void* new_start) {
  size_t move_back = space_between_blocks(new_start, block);
  size_t new_size = block->size + move_back;

  void* new_ptr = memmove(new_start, block, true_size(block));
  MemoryBlock* moved_block = (MemoryBlock*)new_ptr;
  moved_block->size = new_size;
  return moved_block;
}

// combine_b_into_a
void combine_b_into_a(MemoryBlock* block_a, MemoryBlock* block_b) { 
  block_a->next = block_b->next;
  block_a->size += true_size(block_b);
}

MemoryBlock* combine_surrounding_space(MemoryBlock* block) {
  // Expand block forward if there is space
  if (block->prev && space_between_blocks(block->prev, block)) {
    block = expand_back_to(block, next_byte(block->prev));
  }

  // Merge with next blocks if free
  while (block->next && block->next->is_free) {
    // Expand next block foward 
    if (space_between_blocks(block, block->next)) {
      expand_back_to(block->next, next_byte(block));
    }

    // Merge next block into current
    combine_b_into_a(block, block->next);
  }

  return block;
}

