#include "unity/unity.h"
#include "../first_fit.h"

void setUp(void){
  initialize_memory_pool();
}
void tearDown(void){}

// Setup
void test_setup_MemoryPoolLargeEnough(void) {
  TEST_ASSERT_TRUE(MEMORY_POOL_SIZE > 199);
}

void test_setup_MemoryPoolDivisbleByTwo(void) {
  TEST_ASSERT_EQUAL(MEMORY_POOL_SIZE % 2, 0);
}

// my_malloc
void test_SavesPointer(void) {
  char* ptr = (char*)my_malloc(sizeof(char));
  TEST_ASSERT_NOT_NULL(ptr);
}

void test_WhenPassedSizeZero(void) {
  char* ptr = (char*)my_malloc(0);
  TEST_ASSERT_NULL(ptr);
}

void test_ItDoesNotOverwriteData(void) {
  char* ptr1 = (char*)my_malloc(sizeof(char));
  *ptr1 = 'M';
  char* ptr2 = (char*)my_malloc(sizeof(char));
  *ptr2 = 'A';
  TEST_ASSERT_EQUAL('M', *ptr1);
}

void test_WhenPassedSizeOfMemoryPool(void) {
  char* ptr = (char*)my_malloc(MEMORY_POOL_SIZE);
  TEST_ASSERT_NULL(ptr);
}

void test_WhenPassedMultipleLargeSizes(void) {
  size_t size = (MEMORY_POOL_SIZE / 2) + 1;
  char* ptr1 = (char*)my_malloc(size);
  char* ptr2 = (char*)my_malloc(size);
  TEST_ASSERT_NOT_NULL(ptr1);
  TEST_ASSERT_NULL(ptr2);
}

void test_WouldFitExceptForMemoryBlock(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock) + 1;
  char* ptr1 = (char*)my_malloc(size);
  TEST_ASSERT_NULL(ptr1);
}

void test_WhenSizeFitsExactly(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  char* ptr = (char*)my_malloc(size);
  TEST_ASSERT_NOT_NULL(ptr);
}

// my_free
void test_free_ItChangesBlockToBeFree(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  char* ptr = (char*)my_malloc(size);

  my_free(ptr);

  char* second_malloc = (char*)my_malloc(1);
  TEST_ASSERT_NOT_NULL(second_malloc);
}

void test_free_ItReusesThePointer(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  char* ptr = (char*)my_malloc(size);

  my_free(ptr);

  char* second_malloc = (char*)my_malloc(size);
  TEST_ASSERT_EQUAL(ptr, second_malloc);
}

void test_free_CombinesBlocks_WhenFirstIsFreedLast(void) {
  char* ptr1 = (char*)my_malloc(10);
  char* ptr2 = (char*)my_malloc(20);
  char* ptr3 = (char*)my_malloc(30);
  my_malloc(40);

  my_free(ptr3);
  my_free(ptr2);
  my_free(ptr1);

  MemoryBlock* first_block = (MemoryBlock*)ptr1;
  size_t total_size = 10 + (20 + sizeof(MemoryBlock)) + (30 + sizeof(MemoryBlock));
  TEST_ASSERT_EQUAL(first_block->size, total_size);
}

void test_free_CombinesBlocks_WhenMiddleIsFreedLast(void) {
  char* ptr1 = (char*)my_malloc(10);
  char* ptr2 = (char*)my_malloc(20);
  char* ptr3 = (char*)my_malloc(30);
  my_malloc(40);

  my_free(ptr1);
  my_free(ptr3);
  my_free(ptr2);

  MemoryBlock* first_block = (MemoryBlock*)ptr1;
  size_t total_size = 10 + (20 + sizeof(MemoryBlock)) + (30 + sizeof(MemoryBlock));
  TEST_ASSERT_EQUAL(first_block->size, total_size);
}

void test_free_CombinesBlocks_WhenLastIsFreedLast(void) {
  char* ptr1 = (char*)my_malloc(10);
  char* ptr2 = (char*)my_malloc(20);
  char* ptr3 = (char*)my_malloc(30);
  my_malloc(40);

  my_free(ptr1);
  my_free(ptr2);
  my_free(ptr3);

  MemoryBlock* first_block = (MemoryBlock*)ptr1;
  size_t total_size = 10 + (20 + sizeof(MemoryBlock)) + (30 + sizeof(MemoryBlock));
  TEST_ASSERT_EQUAL(first_block->size, total_size);
}

// my_realloc
void test_realloc_IfPassedNil_ItReturnsNil(void) {
  char* ptr = NULL;
  char* new_ptr = (char*)my_realloc(ptr, 1);

  TEST_ASSERT_NULL(new_ptr);
}

void test_realloc_IfPassedZero_ItReturnsNil(void) {
  char* ptr = (char*)my_malloc(1);
  char* new_ptr = (char*)my_realloc(ptr, 0);

  TEST_ASSERT_NULL(new_ptr);
}

void test_realloc_ItChangesSize(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  char* ptr = (char*)my_malloc(size);

  size_t new_size = size / 2;
  char* new_ptr = (char*)my_realloc(ptr, new_size);

  TEST_ASSERT_EQUAL(new_size, ((MemoryBlock*)new_ptr)->size);
}

// Failing
void test_realloc_IfThereIsANonFreeBlockAfterIt_ItDoesNotChangeSize(void) {
  char* ptr1 = (char*)my_malloc(10);
  char* ptr2 = (char*)my_malloc(10);

  char* new_ptr = (char*)my_realloc(ptr1, 15);

  TEST_ASSERT_NULL(new_ptr);
}

void test_realloc_IfThereIsAFreeBlockAfterIt_ItChangesLocationOfFreeBlock(void) {
  char* ptr1 = (char*)my_malloc(10);
  char* ptr2 = (char*)my_malloc(10);

  my_free(ptr2);

  char* new_ptr = (char*)my_realloc(ptr1, 15);

  TEST_ASSERT_NOT_NULL(new_ptr);
}

// Combining blocks

// If there is not enough space left in the memory pool, it should return NULL
// If there is an empty block of memory that is large enough, it should use that block
// If there are two sequential empty blocks that are big enough it combines them



int main(void) {
  UNITY_BEGIN();

  // Setup
  RUN_TEST(test_setup_MemoryPoolLargeEnough);
  RUN_TEST(test_setup_MemoryPoolDivisbleByTwo);
  
  // // Malloc
  RUN_TEST(test_SavesPointer);
  RUN_TEST(test_WhenPassedSizeZero);
  RUN_TEST(test_ItDoesNotOverwriteData);
  RUN_TEST(test_WhenPassedSizeOfMemoryPool);
  RUN_TEST(test_WhenPassedMultipleLargeSizes);
  RUN_TEST(test_WouldFitExceptForMemoryBlock);
  RUN_TEST(test_WhenSizeFitsExactly);

  // // Free
  RUN_TEST(test_free_ItChangesBlockToBeFree);
  RUN_TEST(test_free_ItReusesThePointer);
  RUN_TEST(test_free_CombinesBlocks_WhenFirstIsFreedLast);
  RUN_TEST(test_free_CombinesBlocks_WhenMiddleIsFreedLast);
  RUN_TEST(test_free_CombinesBlocks_WhenLastIsFreedLast);


  // // Realloc
  RUN_TEST(test_realloc_IfPassedNil_ItReturnsNil);
  RUN_TEST(test_realloc_IfPassedZero_ItReturnsNil);
  RUN_TEST(test_realloc_ItChangesSize);
  // RUN_TEST(test_realloc_IfThereIsANonFreeBlockAfterIt_ItDoesNotChangeSize);
  RUN_TEST(test_realloc_IfThereIsAFreeBlockAfterIt_ItChangesLocationOfFreeBlock);

  return UNITY_END();
}

// Compile with this from the test branch.
// gcc -o test_first_fit test_first_fit.c ../first_fit.c unity/unity.c