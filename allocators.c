#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h> 

#include "allocators.h"

// Functions
static void* byte_after_memory_pool();
static void* next_byte(MemoryBlock* block);
static size_t space_between_blocks(MemoryBlock* a, MemoryBlock* b);
static size_t true_size(MemoryBlock* block);
static MemoryBlock* expand_back_to(MemoryBlock* block, void* new_start);
static void combine_b_into_a(MemoryBlock* a, MemoryBlock* b);
static MemoryBlock* combine_surrounding_space(MemoryBlock* block);
static void create_block_if_space_after(MemoryBlock* block);
static void create_memory_block_after(MemoryBlock* block, size_t size);
static void initialize_memory_block(MemoryBlock* block, size_t size, int is_free, MemoryBlock* prev, MemoryBlock* next);
static void allocate_to(MemoryBlock* block, size_t size);
static int byte_index(void* ptr);
static void print_memory_block(MemoryBlock* block);

// Variables/State
static unsigned char* memory_pool = NULL;
static MemoryBlock* first_block = NULL;
static size_t memory_pool_size = 0;

void* initialize_memory_pool(size_t size) {
  if (memory_pool != NULL) {
    free_memory_pool();
  }

  memory_pool = malloc(size);
  if (memory_pool == NULL) {
    return NULL;
  }

  memory_pool_size = size;

  first_block = (MemoryBlock*)memory_pool;
  initialize_memory_block(first_block, size - sizeof(MemoryBlock), 1, NULL, NULL);

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
        allocate_to(current, size);
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
  size_t bytes_to_copy = true_size(block);

  MemoryBlock* moved_block = combine_surrounding_space(block);

  if (moved_block->size >= size) {
    allocate_to(moved_block, size);
    return (void*)(moved_block + 1);
  } else {
    void* new_ptr = cool_malloc(size);

    if (new_ptr == NULL) {
      return NULL;
    }

    memmove(moved_block, (void*)((MemoryBlock*)(new_ptr) - 1), bytes_to_copy);
    return new_ptr;
  }
}

// Private functions
static void* byte_after_memory_pool() {
  return (void*)((char*)first_block + memory_pool_size);
}

static void* next_byte(MemoryBlock* block) {
  return (char*)(block + 1) + block->size;
}

static size_t space_between_blocks(MemoryBlock* block1, MemoryBlock* block2) {
  void* start_location = next_byte(block1);

  if (start_location >= byte_after_memory_pool()) {
    return 0;
  }

  void* end_location;

  if (block2 == NULL) {
    end_location = byte_after_memory_pool();
  } else {
    end_location = (void*)block2;
  }

  return (size_t)(end_location - start_location);
}

static size_t true_size(MemoryBlock* block) {
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

static void combine_b_into_a(MemoryBlock* block_a, MemoryBlock* block_b) { 
  block_a->next = block_b->next;
  block_a->size += true_size(block_b);
}

static MemoryBlock* combine_surrounding_space(MemoryBlock* block) {
  // Expand block to earlier byte if there is space
  if (block->prev && space_between_blocks(block->prev, block)) {
    block = expand_back_to(block, next_byte(block->prev));
  }

  // Merge with next blocks if free
  while (block->next && block->next->is_free) {
    // Expand free block to byte after current
    if (space_between_blocks(block, block->next)) {
      block->next = expand_back_to(block->next, next_byte(block));
    }

    // Merge next block into current
    combine_b_into_a(block, block->next);
  }

  return block;
}

static void create_block_if_space_after(MemoryBlock* block) {
  size_t space_between_current_and_next = space_between_blocks(block, block->next);

  // If there is space for a memory block and one byte
  if (space_between_current_and_next > sizeof(MemoryBlock)) {
    size_t new_block_size = space_between_current_and_next - sizeof(MemoryBlock);
    create_memory_block_after(block, new_block_size);
  }
}

static void create_memory_block_after(MemoryBlock* block, size_t size) {
  MemoryBlock* next_block = next_byte(block);
  initialize_memory_block(next_block, size, 1, block, block->next);
  block->next = next_block;
}

static void initialize_memory_block(MemoryBlock* block, size_t size, int is_free, MemoryBlock* prev, MemoryBlock* next) {
  block->size = size;
  block->is_free = is_free;
  block->prev = prev;
  block->next = next;
}

static void allocate_to(MemoryBlock* block, size_t size) {
  block->is_free = 0;
  block->size = size;

  create_block_if_space_after(block);
}


// Private functions for debugging
static void print_memory_block(MemoryBlock* block) {
  printf("MemoryBlock: %p\n", block);
  printf("  size: %zu\n", block->size);
  printf("  is_free: %d\n", block->is_free);
  printf("  prev: %p\n", block->prev);
  printf("  next: %p\n", block->next);
}

static int byte_index(void* ptr) {
  return (char*)ptr - (char*)first_block;
}
