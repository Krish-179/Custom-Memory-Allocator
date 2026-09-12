#include "allocator.h"
#include <stdio.h>

int main(void) {
  void *p1 = my_malloc(50);
  void *p2 = my_malloc(100); // this one will get split later
  void *p3 = my_malloc(
      50); // sits right after p2 — the "after" block that should get updated
           //
  my_free(p2);    // p2 alone, isolated, now FREE, size=100
  print_blocks(); // baseline — confirm p3's prev_size = 100 here
                  //
  void *x =
      my_malloc(20); // should reuse+split p2: used(20) + free_remainder(~56)
  print_blocks();

  return 0;
}
