#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h> 

#include "first_fit.h"

static MemoryBlock* combine_surrounding_space(MemoryBlock* block);
static size_t space_between_blocks(MemoryBlock* a, MemoryBlock* b);
static void* next_byte(MemoryBlock* block);
static MemoryBlock* expand_back_to(MemoryBlock* block, void* new_start);
static void combine_b_into_a(MemoryBlock* a, MemoryBlock* b);
static void* last_byte();
static int byte_index(void* ptr);
static void* create_memory_block_after(MemoryBlock* block, size_t size);

static unsigned char* memory_pool = NULL;  // Dynamic memory pool
static MemoryBlock* first_block = NULL;      // Free list head
static size_t memory_pool_size = 0;   

void* initialize_memory_pool(size_t size) {
  if (memory_pool != NULL) {
    free_memory_pool();
  }

  memory_pool = malloc(size);
  memory_pool_size = size;

  first_block = (MemoryBlock*)memory_pool;
  first_block->size = size - sizeof(MemoryBlock);
  first_block->is_free = 1;
  first_block->next = NULL;
  first_block->prev = NULL;

  return NULL;
}

void* free_memory_pool() {
  free(memory_pool);
  memory_pool = NULL;
  first_block = NULL;
  memory_pool_size = 0;
  return NULL;
}

void* cool_malloc(size_t size) {
  if (size == 0) {
    return NULL;
  }

  MemoryBlock* current = first_block;
  while (current) {
    if (current->is_free) {
      current = combine_surrounding_space(current);
      if (current->size >= size) {
        current->is_free = 0;
        current->size = size;

        // If there is space for a memory block
        size_t space_between_current_and_next = space_between_blocks(current, current->next);
        if (space_between_current_and_next > sizeof(MemoryBlock)) {
          size_t new_block_size = space_between_current_and_next - sizeof(MemoryBlock);
          create_memory_block_after(current, new_block_size);
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
static int byte_index(void* ptr) {
  return (char*)ptr - (char*)first_block;
}

static void* last_byte() {
  return (void*)((char*)first_block + memory_pool_size);
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

static MemoryBlock* expand_back_to(MemoryBlock* block, void* new_start) {
  size_t move_back = space_between_blocks(new_start, block);
  size_t new_size = block->size + move_back;

  void* new_ptr = memmove(new_start, block, true_size(block));
  MemoryBlock* moved_block = (MemoryBlock*)new_ptr;
  moved_block->size = new_size;
  return moved_block;
}

// combine_b_into_a
static void combine_b_into_a(MemoryBlock* block_a, MemoryBlock* block_b) { 
  block_a->next = block_b->next;
  block_a->size += true_size(block_b);
}

static MemoryBlock* combine_surrounding_space(MemoryBlock* block) {
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

static void* create_memory_block_after(MemoryBlock* block, size_t size) {
  MemoryBlock* next_block = next_byte(block);
  next_block->next = block->next;
  next_block->prev = block;
  next_block->size = size;
  next_block->is_free = 1;
  block->next = next_block;
  return NULL;
}

