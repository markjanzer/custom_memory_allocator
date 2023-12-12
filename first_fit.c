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
void* first_byte();
void* last_byte();
int byte_index(void* ptr);

unsigned char memory_pool[MEMORY_POOL_SIZE];
MemoryBlock* free_list = NULL;

void initialize_memory_pool() {
  free_list = (MemoryBlock*)memory_pool;
  free_list->size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  free_list->is_free = 1;
  free_list->next = NULL;
  free_list->prev = NULL;
}

// I want to implement these
// void* first_byte = free_list;
// void* last_byte = (char*)free_list + MEMORY_POOL_SIZE - 1;

void* cool_malloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  MemoryBlock* current = free_list;
  while (current) {
    if (current->is_free) {
      current = combine_surrounding_space(current);
      if (current->size >= size) {
        current->is_free = 0;
        current->size = size;

        // If there is space for a memory block
        size_t space_between_current_and_next = space_between_blocks(current, current->next);
        if (space_between_current_and_next > sizeof(MemoryBlock)) {
          // Create a new free memory block

          // Refactor using this after tests pass
          // create_memory_block_after(block);
          MemoryBlock* next_block = next_byte(current);
          next_block->next = current->next;
          next_block->prev = current;
          next_block->size = space_between_current_and_next - sizeof(MemoryBlock);
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

  MemoryBlock* block = ((MemoryBlock*)ptr) - 1;
  block->is_free = 1;

  return NULL;
}

void* cool_realloc(void* ptr, size_t size) {
  if (ptr == NULL || size == 0) {
    return NULL;
  }

  MemoryBlock* block = ((MemoryBlock*)ptr) - 1;

  size_t old_size = block->size;
  MemoryBlock* moved_block = combine_surrounding_space(block);

  if (moved_block->size >= size) {
    moved_block->size = size;
    return (void*)(moved_block + 1);
  } else {
    // This won't work with concurrency
    cool_free((moved_block + 1));
    void* new_ptr = cool_malloc(size);

    if (new_ptr == NULL) {
      return NULL;
    }

    memmove(moved_block, (void*)((MemoryBlock*)(new_ptr) - 1), old_size);
    return new_ptr;
  }
}

// HELPER FUNCTIONS
// Might not be using this one
void* first_byte() {
  return (void*)free_list;
}

int byte_index(void* ptr) {
  return (char*)ptr - (char*)first_byte();
}

void* last_byte() {
  return (void*)((char*)free_list + MEMORY_POOL_SIZE);
}

void* next_byte(MemoryBlock* block) {
  return (char*)(block + 1) + block->size;
}

size_t space_between_blocks(MemoryBlock* a, MemoryBlock* b) {
  char* start_location = (char*)(next_byte(a));

  if (start_location >= (char*)last_byte()) {
    return 0;
  }

  char* end_location;

  if (b == NULL) {
    end_location = last_byte();
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

