#include "unity/unity.h"
#include "../allocators.h"

size_t memory_pool_size;

void setUp(void){
  memory_pool_size = 200;
  initialize_memory_pool(memory_pool_size);
}
void tearDown(void){
  free_memory_pool();
}

// cool_malloc
void test_malloc_SavesPointer(void) {
  void* ptr = cool_malloc(1);
  TEST_ASSERT_NOT_NULL(ptr);
}

void test_malloc_WhenPassedSizeZero(void) {
  void* ptr = cool_malloc(0);
  TEST_ASSERT_NULL(ptr);
}

void test_malloc_ItDoesNotOverwriteData(void) {
  char* ptr1 = (char*)cool_malloc(sizeof(char));
  *ptr1 = 'M';
  char* ptr2 = (char*)cool_malloc(sizeof(char));
  TEST_ASSERT_NOT_NULL(ptr2);
  *ptr2 = 'A';

  TEST_ASSERT_EQUAL('M', *ptr1);
}

void test_malloc_ItCanHaveThreeALlocations(void) {
  void* ptr1 = cool_malloc(1);
  void* ptr2 = cool_malloc(1);
  void* ptr3 = cool_malloc(1);
  TEST_ASSERT_NOT_NULL(ptr2);
  TEST_ASSERT_NOT_NULL(ptr3);
}

void test_malloc_WhenPassedSizeOfMemoryPool(void) {
  void* ptr = cool_malloc(memory_pool_size);
  TEST_ASSERT_NULL(ptr);
}

void test_malloc_WhenPassedMultipleLargeSizes(void) {
  size_t size = (memory_pool_size / 2) + 1;
  void* ptr1 = cool_malloc(size);
  void* ptr2 = cool_malloc(size);
  TEST_ASSERT_NOT_NULL(ptr1);
  TEST_ASSERT_NULL(ptr2);
}

void test_malloc_ItCanSplitMemoryIntoQuarters(void) {
  size_t quarter = (memory_pool_size / 4) - sizeof(MemoryBlock);
  void* ptr1 = cool_malloc(quarter);
  void* ptr2 = cool_malloc(quarter);
  void* ptr3 = cool_malloc(quarter);
  void* ptr4 = cool_malloc(quarter);

  TEST_ASSERT_NOT_NULL(ptr1);
  TEST_ASSERT_NOT_NULL(ptr2);
  TEST_ASSERT_NOT_NULL(ptr3);
  TEST_ASSERT_NOT_NULL(ptr4);
}

void test_malloc_WouldFitExceptForMemoryBlock(void) {
  size_t size = memory_pool_size - sizeof(MemoryBlock) + 1;
  void* ptr1 = cool_malloc(size);
  TEST_ASSERT_NULL(ptr1);
}

void test_malloc_WhenSizeFitsExactly(void) {
  size_t size = memory_pool_size - sizeof(MemoryBlock) - 1;
  void* ptr = cool_malloc(size);
  TEST_ASSERT_NOT_NULL(ptr);
}

void test_malloc_ItMakesSpaceByCombiningFreeBlocks(void) {
  size_t quarter = memory_pool_size / 4 - sizeof(MemoryBlock);
  size_t half = memory_pool_size / 2 - sizeof(MemoryBlock);
  void* ptr1 = cool_malloc(quarter);
  void* ptr2 = cool_malloc(quarter);
  void* ptr3 = cool_malloc(quarter);
  void* ptr4 = cool_malloc(quarter);

  cool_free(ptr2);
  cool_free(ptr3);

  void* ptr5 = cool_malloc(half);

  TEST_ASSERT_NOT_NULL(ptr5);
}

// cool_free
void test_free_ItAllowsTheSpaceToBeUsed(void) {
  size_t size = memory_pool_size - sizeof(MemoryBlock);
  void* ptr = cool_malloc(size);

  cool_free(ptr);

  void* second_malloc = cool_malloc(1);
  TEST_ASSERT_NOT_NULL(second_malloc);
}

void test_free_ItReusesThePointer(void) {
  size_t size = memory_pool_size - sizeof(MemoryBlock);
  void* ptr = cool_malloc(size);

  cool_free(ptr);

  void* second_malloc = cool_malloc(size);
  TEST_ASSERT_EQUAL(ptr, second_malloc);
}

// cool_realloc
void test_realloc_IfPassedNil_ItReturnsNil(void) {
  void* ptr = NULL;
  void* new_ptr = cool_realloc(ptr, 1);

  TEST_ASSERT_NULL(new_ptr);
}

void test_realloc_IfPassedZero_ItReturnsNil(void) {
  void* ptr = cool_malloc(1);
  void* new_ptr = cool_realloc(ptr, 0);

  TEST_ASSERT_NULL(new_ptr);
}

void test_realloc_ItChangesSize(void) {
  size_t size = memory_pool_size - sizeof(MemoryBlock);
  void* ptr = cool_malloc(size);

  size_t new_size = size / 2;
  void* new_ptr = cool_realloc(ptr, new_size);
  MemoryBlock* block = ((MemoryBlock*)new_ptr) - 1;

  TEST_ASSERT_EQUAL(block->size, new_size);
}

void test_realloc_ItWorksWhenExistingBlockSpaceIsntLargeEnough(void) {
  size_t quarter = (memory_pool_size / 4) - sizeof(MemoryBlock);
  void* ptr1 = cool_malloc(quarter);
  void* ptr2 = cool_malloc(quarter);

  void* ptr3 = cool_realloc(ptr1, quarter * 2);

  TEST_ASSERT_NOT_NULL(ptr3);
}

void test_realloc_ItCompactsAndExpandsBackwards(void) {
  size_t quarter = (memory_pool_size / 4) - sizeof(MemoryBlock);
  void* ptr1 = cool_malloc(quarter);
  void* ptr2 = cool_malloc(quarter);
  void* ptr3 = cool_malloc(quarter);
  void* ptr4 = cool_malloc(quarter);

  cool_free(ptr2);
  cool_free(ptr3);

  void* ptr5 = cool_realloc(ptr4, quarter * 3);

  TEST_ASSERT_NOT_NULL(ptr5);
}

int main(void) {
  UNITY_BEGIN();

  // Malloc
  RUN_TEST(test_malloc_WhenPassedSizeZero);
  RUN_TEST(test_malloc_SavesPointer);
  RUN_TEST(test_malloc_ItDoesNotOverwriteData);
  RUN_TEST(test_malloc_ItCanHaveThreeALlocations);
  RUN_TEST(test_malloc_WhenPassedSizeOfMemoryPool);
  RUN_TEST(test_malloc_WhenPassedMultipleLargeSizes);
  RUN_TEST(test_malloc_ItCanSplitMemoryIntoQuarters);
  RUN_TEST(test_malloc_WouldFitExceptForMemoryBlock);
  RUN_TEST(test_malloc_WhenSizeFitsExactly);
  RUN_TEST(test_malloc_ItMakesSpaceByCombiningFreeBlocks);

  // // Free
  RUN_TEST(test_free_ItAllowsTheSpaceToBeUsed);
  RUN_TEST(test_free_ItReusesThePointer);

  // // Realloc
  RUN_TEST(test_realloc_IfPassedNil_ItReturnsNil);
  RUN_TEST(test_realloc_IfPassedZero_ItReturnsNil);
  RUN_TEST(test_realloc_ItChangesSize);
  RUN_TEST(test_realloc_ItWorksWhenExistingBlockSpaceIsntLargeEnough);
  RUN_TEST(test_realloc_ItCompactsAndExpandsBackwards);

  return UNITY_END();
}