#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>

typedef struct MemoryBlock {
  size_t size;
  int is_free;
  struct MemoryBlock* next;
} MemoryBlock;

#define MEMORY_POOL_SIZE 100
unsigned char memory_pool[MEMORY_POOL_SIZE];
MemoryBlock* free_list = NULL;

void initialize_memory_pool() {
  free_list = (MemoryBlock*)memory_pool;
  free_list->size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  free_list->is_free = 1;
  free_list->next = NULL;
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

      // Create new block to store remaining memory
      MemoryBlock* next_block = (MemoryBlock*)((char*)(current + 1) + size);
      next_block->size = current->size - sizeof(MemoryBlock);
      next_block->is_free = 1;
      next_block->next = NULL;

      current->next = next_block;
      return (void*)(current + 1);
    }
    current = current->next;
  }

  return NULL;
}

int main() {
  initialize_memory_pool();

  printf("%p\n", free_list);

  char* ptr1 = (char*)my_malloc(sizeof(char));
  *ptr1 = 'M';
  char* ptr2 = (char*)my_malloc(sizeof(char));
  *ptr2 = 'A';
  char* ptr3 = (char*)my_malloc(sizeof(char));
  *ptr3 = 'R';
  char* ptr4 = (char*)my_malloc(sizeof(char));
  *ptr4 = 'K';
  char* ptr5 = (char*)my_malloc(sizeof(char));
  *ptr5 = 'J';

  printf("sizeof(MemoryBlock) = %lu\n", sizeof(MemoryBlock));

  if (ptr1 != NULL) {
    printf(":D\n");
  } else {
    printf("Allocation 1 failed\n");
  }

  if (ptr2 != NULL) {
    printf(":D\n");
  } else {
    printf("Allocation 2 failed\n");
  }

  if (ptr3 != NULL) {
    printf(":D\n");
  } else {
    printf("Allocation 3 failed\n");
  }

  if (ptr4 != NULL) {
    printf(":D\n");
  } else {
    printf("Allocation 4 failed\n");
  }

  if (ptr5 != NULL) {
    printf(":D\n");
  } else {
    printf("Allocation 5 failed\n");
  }

  if (*ptr1 == 'M') {
    printf(":D\n");
  } else {
    printf("Data 1 is incorrect\n");
  }

  if (*ptr2 == 'A') {
    printf(":D\n");
  } else {
    printf("Data 2 is incorrect\n");
  }

  if (*ptr3 == 'R') {
    printf(":D\n");
  } else {
    printf("Data 3 is incorrect\n");
  }

  if (*ptr4 == 'K') {
    printf(":D\n");
  } else {
    printf("Data 4 is incorrect\n");
  }

  if (*ptr5 == 'J') {
    printf(":D\n");
  } else {
    printf("Data 5 is incorrect\n");
  }

  printf("%p\n", free_list);
  printf("%p\n", ptr5);

  uintptr_t pointer_start = (uintptr_t)(free_list);
  uintptr_t pointer_end = (uintptr_t)(ptr5);
  ptrdiff_t pointer_diff = pointer_end - pointer_start;
  printf("Number of bytes between pointers: %td\n", pointer_diff);

  if (pointer_diff >= MEMORY_POOL_SIZE) {
    printf("FAIL: exceeded memory pool with %td bytes\n", pointer_diff);
  } else {
    printf(":D\n");
  }
  

  return 0;
}
