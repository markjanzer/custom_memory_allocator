// Allocate space for an array of 10 integers
// give them the values of 10 to 100 by 10
// print them out
// print out the address of the array

#include <stdlib.h>
#include <stdio.h>

int main() {
  // int int_size = sizeof(int);
  // int array_length = 10;

  // int *a = calloc( int_size, array_length );

  // for (int i = 0; i < 10; i++) a[i] = 10 * (i + 1);

  // for (int i = 0; i < 10; i++)
  //   printf("%d\n", a[i]);

  // printf("%p", a);

  // free(a);

  int a = 10;
  int* b = &a;

  printf("%d\n", a);
  printf("%p\n", b);
  printf("%d\n", *b);

  return 0;
}
