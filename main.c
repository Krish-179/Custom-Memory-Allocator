#include "allocator.h"
#include <stdio.h>

int main(void) {
  printf("=== allocate four blocks ===\n");
  void *p1 = my_malloc(50);
  void *p2 = my_malloc(50);
  void *p3 = my_malloc(50);
  void *p4 = my_malloc(50);
  print_blocks();

  printf("\n=== free p3 ===\n");
  my_free(p3);
  print_blocks();

  printf("\n=== free p4 ===\n");
  my_free(p4);
  print_blocks();

  printf("\n=== free p2 ===\n");
  my_free(p2);
  print_blocks();

  printf("\n=== free p1 ===\n");
  my_free(p1);
  print_blocks();

  printf("\n=== malloc(150) ===\n");
  void *big = my_malloc(150);
  printf("p1=%p big=%p\n", p1, big);
  print_blocks();

  return 0;
}
