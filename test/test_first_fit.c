#include "unity/unity.h"
#include "../first_fit.h"

void setUp(void){
  initialize_memory_pool();
}
void tearDown(void){
  // initialize_memory_pool();
}

// Setup
void test_setup_MemoryPoolLargeEnough(void) {
  TEST_ASSERT_TRUE(MEMORY_POOL_SIZE > 199);
}

void test_setup_MemoryPoolDivisbleByTwo(void) {
  TEST_ASSERT_EQUAL(MEMORY_POOL_SIZE % 2, 0);
}

// cool_malloc
void test_SavesPointer(void) {
  char* ptr = (char*)cool_malloc(sizeof(char));
  TEST_ASSERT_NOT_NULL(ptr);
}

void test_WhenPassedSizeZero(void) {
  char* ptr = (char*)cool_malloc(0);
  TEST_ASSERT_NULL(ptr);
}

void test_ItDoesNotOverwriteData(void) {
  // int num = number_of_memory_blocks();
  // printf("Number of Memory Blocks: %d\n", num);
  char* ptr0 = (char*)cool_malloc(sizeof(char));
  
  char* ptr1 = (char*)cool_malloc(sizeof(char));
  *ptr1 = 'M';
  // This is returning NULL :thinking:
  char* ptr2 = (char*)cool_malloc(sizeof(char));
  TEST_ASSERT_NOT_NULL(ptr2);
  *ptr2 = 'A';
  TEST_ASSERT_EQUAL('M', *ptr1);
}

void test_ItCanHaveThreeALlocations(void) {
  char* ptr1 = (char*)cool_malloc(sizeof(char));
  char* ptr2 = (char*)cool_malloc(sizeof(char));
  char* ptr3 = (char*)cool_malloc(sizeof(char));
  TEST_ASSERT_NOT_NULL(ptr2);
  TEST_ASSERT_NOT_NULL(ptr3);
}

void test_WhenPassedSizeOfMemoryPool(void) {
  char* ptr = (char*)cool_malloc(MEMORY_POOL_SIZE);
  TEST_ASSERT_NULL(ptr);
}

void test_WhenPassedMultipleLargeSizes(void) {
  size_t size = (MEMORY_POOL_SIZE / 2) + 1;
  char* ptr1 = (char*)cool_malloc(size);
  char* ptr2 = (char*)cool_malloc(size);
  TEST_ASSERT_NOT_NULL(ptr1);
  TEST_ASSERT_NULL(ptr2);
}

void test_WouldFitExceptForMemoryBlock(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock) + 1;
  char* ptr1 = (char*)cool_malloc(size);
  TEST_ASSERT_NULL(ptr1);
}

void test_WhenSizeFitsExactly(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock) - 1;
  char* ptr = (char*)cool_malloc(size);
  TEST_ASSERT_NOT_NULL(ptr);
}

// cool_free
void test_free_ItAllowsTheSpaceToBeUsed(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  char* ptr = (char*)cool_malloc(size);

  cool_free(ptr);

  char* second_malloc = (char*)cool_malloc(1);
  TEST_ASSERT_NOT_NULL(second_malloc);
}

void test_free_ItReusesThePointer(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  char* ptr = (char*)cool_malloc(size);

  cool_free(ptr);

  char* second_malloc = (char*)cool_malloc(size);
  TEST_ASSERT_EQUAL(ptr, second_malloc);
}

// cool_realloc
void test_realloc_IfPassedNil_ItReturnsNil(void) {
  char* ptr = NULL;
  char* new_ptr = (char*)cool_realloc(ptr, 1);

  TEST_ASSERT_NULL(new_ptr);
}

void test_realloc_IfPassedZero_ItReturnsNil(void) {
  char* ptr = (char*)cool_malloc(1);
  char* new_ptr = (char*)cool_realloc(ptr, 0);

  TEST_ASSERT_NULL(new_ptr);
}

void test_realloc_ItChangesSize(void) {
  size_t size = MEMORY_POOL_SIZE - sizeof(MemoryBlock);
  char* ptr = (char*)cool_malloc(size);

  size_t new_size = size / 2;
  char* new_ptr = (char*)cool_realloc(ptr, new_size);
  MemoryBlock* block = ((MemoryBlock*)new_ptr) - 1;

  TEST_ASSERT_EQUAL(block->size, new_size);
}

void test_realloc_ItCompactsAndExpandsBackwards(void) {
  size_t quarter = (MEMORY_POOL_SIZE / 4) - sizeof(MemoryBlock);
  void* ptr1 = cool_malloc(quarter);
  void* ptr2 = cool_malloc(quarter);
  void* ptr3 = cool_malloc(quarter);
  void* ptr4 = cool_malloc(quarter);

  cool_free(ptr2);
  cool_free(ptr3);

  void* ptr5 = cool_realloc(ptr4, quarter * 3);

  TEST_ASSERT_NOT_NULL(ptr5);
}

// Combining blocks

void test_scenario_ItMakesSpaceByCombiningBlocks(void) {
  size_t quarter = MEMORY_POOL_SIZE / 4;
  size_t half = MEMORY_POOL_SIZE / 2;
  void* ptr1 = cool_malloc(quarter);
  void* ptr2 = cool_malloc(quarter);
  void* ptr3 = cool_malloc(quarter);
  void* ptr4 = cool_malloc(quarter);

  cool_free(ptr2);
  cool_free(ptr4);

  void* ptr5 = cool_malloc(half);

  TEST_ASSERT_NOT_NULL(ptr5);
}

// If there is not enough space left in the memory pool, it should return NULL
// If there is an empty block of memory that is large enough, it should use that block
// If there are two sequential empty blocks that are big enough it combines them



int main(void) {
  UNITY_BEGIN();

  // Setup
  // RUN_TEST(test_setup_MemoryPoolLargeEnough);
  // RUN_TEST(test_setup_MemoryPoolDivisbleByTwo);
  
  // // Malloc
  // RUN_TEST(test_WhenPassedSizeZero);
  // RUN_TEST(test_SavesPointer);
  // RUN_TEST(test_ItDoesNotOverwriteData);
  // RUN_TEST(test_ItCanHaveThreeALlocations);

  // RUN_TEST(test_WhenPassedSizeOfMemoryPool);
  // RUN_TEST(test_WhenPassedMultipleLargeSizes);
  // RUN_TEST(test_WouldFitExceptForMemoryBlock);
  // RUN_TEST(test_WhenSizeFitsExactly);

  // // Free
  // RUN_TEST(test_free_ItAllowsTheSpaceToBeUsed);
  // RUN_TEST(test_free_ItReusesThePointer);

  // // Realloc
  // RUN_TEST(test_realloc_IfPassedNil_ItReturnsNil);
  // RUN_TEST(test_realloc_IfPassedZero_ItReturnsNil);
  // RUN_TEST(test_realloc_ItChangesSize);
  RUN_TEST(test_realloc_ItCompactsAndExpandsBackwards);

  // Tricky scenarios
  RUN_TEST(test_scenario_ItMakesSpaceByCombiningBlocks);

  return UNITY_END();
}

// Compile with this from the test branch.
// gcc -o test_first_fit test_first_fit.c ../first_fit.c unity/unity.c