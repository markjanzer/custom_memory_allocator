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
    // printf("NEW MALLOC LOOP\n");
    // printf("given size: %zu\n", size);
    // printf("current: %d\n", byte_index(current));
    // printf("current->is_free: %d\n", current->is_free);
    // printf("current->size: %zu\n", current->size);
    // printf("current->next: %d\n", byte_index(current->next));
    // printf("current->prev: %d\n", byte_index(current->prev));
    if (current->is_free) {
      current = combine_surrounding_space(current);

      // printf("current->size: %zu\n", current->size);
      // printf("size: %zu\n", size);
      // printf("current->size >= size: %d\n", current->size >= size);
      if (current->size >= size) {
        current->is_free = 0;
        current->size = size;

        // If there is space for a memory block
        // printf("current: %d\n", byte_index(current));
        // printf("current->next: %d\n", byte_index(current->next));
        size_t space_between_current_and_next = space_between_blocks(current, current->next);
        if (space_between_current_and_next > sizeof(MemoryBlock)) {
          // Create a new free memory block

          // Refactor using this after tests pass
          // create_memory_block_after(block);
          MemoryBlock* next_block = next_byte(current);
          next_block->next = current->next;
          next_block->prev = current;
          // printf("next_block %d\n", byte_index(next_block));
          // printf("next_block->next %d\n", byte_index(next_block->next));
          next_block->size = space_between_current_and_next - sizeof(MemoryBlock);
          next_block->is_free = 1;
          current->next = next_block;
          // printf("next_block: %d\n", byte_index(next_block));
          // printf("next_block->next: %d\n", byte_index(next_block->next));
          // printf("next_block->size: %zu\n", next_block->size);
          // printf("created new block: %d\n", byte_index(next_block));
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
  if (size == 0 || ptr == NULL) {
    return NULL;
  }

  MemoryBlock* block = ((MemoryBlock*)ptr) - 1;

  size_t old_size = block->size;
  MemoryBlock* moved_block = combine_surrounding_space(block);

  if (moved_block->size >= size) {
    printf("in moved_block->size >= size\n");
    moved_block->size = size;
    return (void*)(moved_block + 1);
  } else {
    printf("in here");
    // This won't work with concurrency
    cool_free(moved_block);
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
  return (void*)((char*)free_list + MEMORY_POOL_SIZE - 1);
}

void* next_byte(MemoryBlock* block) {
  return (char*)(block + 1) + block->size;
}

size_t space_between_blocks(MemoryBlock* a, MemoryBlock* b) {
  // printf("SPACE BETWEEN BLOCKS\n");
  char* start_location = (char*)(next_byte(a));

  // printf("SBB start_location: %d\n", byte_index(start_location));
  // printf("SBB last_byte: %p\n", last_byte());

  if (start_location >= (char*)last_byte()) {
    return 0;
  }

  char* end_location;

  // printf("SBB: b == NULL %d\n", b == NULL);

  if (b == NULL) {
    end_location = last_byte();
    // printf("SBB end_location: %d\n", byte_index(end_location));
    // printf("SBB result: %zu\n", (size_t)(end_location - start_location));
  } else {
    end_location = (char*)b;
  }

  // printf("SBB end_location: %p\n", end_location);
  // printf("SBB result: %zu\n", (size_t)(end_location - start_location)); 

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

