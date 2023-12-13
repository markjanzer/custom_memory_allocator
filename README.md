# Custom Memory Allocator
In an effor to learn C and memory management I wrote my own memory management functions, `cool_malloc`, `cool_free`, and `cool_realloc`.

Please don't use this software, it most definitely has bugs.

## Installation/Usage
1. Copy `allocators.c` and `allocators.h` into your project directory.
2. Include `allocators.c` in your compilation command, along with your other source files. For example:
```
gcc -o your_program your_program.c allocators.c
```
3. Use in your code
```
#include "allocators.h"

int main() {
  initialize_memory_pool(1024 * 1024); // Specify the number of bytes you want allocated
  void* my_block = cool_malloc(1024); // Allocate memory
  my_block = cool_realloc(my_block, 2048); // Reallocate

  // Your code here

  cool_free(my_block); // Free memory

  free_memory_pool(); // Free pool when done
}
```

## Strategy
The allocators in this project use a first fit strategy. Compaction and coalescing are done on `malloc` and `realloc` to ensure space is used efficiently.

When the memory pool is first initialized, one block is created that encapsulates the entire span of the memory pool.

### `cool_malloc`
Malloc finds the first free block, and then if there is any unaccounted for space in front or behind it, or if it is adjacent to any other free blocks, then it combines them.
After these compression operations, it checks to see if it is large enough to accomodate the given size. If not it moves onto the next block
But if it is large enough then it sets itself to not free and shrinks its size to match the requirements. After shrinking, it checks the distance to the next block, and if it is large enough to hold any data (`sizeof(MemoryBlock) + 1` bytes), then it creates a block to take up that space.

### `cool_free`
Frees the block

### `cool_realloc`
Similar to `cool_malloc`, `cool_realloc` starts by combining the unaccounted for space around the given block.
If the existing space is large enough then it uses the existing space (also creating a block after it to reclaim any unused space). However if the current space is not big enough then it calls `cool_malloc`, and then moves the memory after the fact.

## Learnings
- First project in C! I feel much more comfortable in the language already
- I learned a lot about memory management! There are a lot of different approaches you can take, and there are a lot of ways to mess things up
- I now have a further understanding as why off by one errors are one of the hardest things in programming
- Used Unity for the first time. I appreciate it's simplicity, though I will look into some of the tooling (like ceedling) to see if that makes it easier to use
- Errors weren't super helpful, it was useful to build my own helper function to help debug the systems
- Managing and containing state was interesting. My initial setups had shared global variables. Coming from Ruby, I started to think of each file as a class, with state (variables), public methods (shared functions), and private methods (static functions). This mostly worked out.

