#include "allocator.h"
#include <stdio.h>

int main(void) {
  printf("--- malloc(100) ---\n");
  void *a = my_malloc(100);
  print_blocks(); // expect: 1 block, size=100, USED
  printf("--- free(a) ---\n");
  my_free(a);
  print_blocks(); // expect: 1 block, size=100, FREE

  printf("--- malloc(20) — should split ---\n");
  void *b = my_malloc(20);
  print_blocks(); // expect: 2 blocks now — size=20 USED, size~=64 FREE

  printf("a=%p b=%p (should match)\n", a, b);

  printf("--- malloc(50) — should reuse the split leftover ---\n");
  void *c = my_malloc(50);
  print_blocks(); // expect: 3 blocks — size=20 USED, size=50 USED, size~=14
                  // FREE (or no split if leftover too small)
  return 0;
}	
