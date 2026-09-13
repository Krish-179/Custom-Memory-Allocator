#include "allocator.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define SECTION(name) printf("\n=== %s ===\n", name)

int main(void) {

  SECTION("1. Basic malloc + free");
  void *a = my_malloc(100);
  assert(a != NULL);
  print_blocks();
  my_free(a);
  print_blocks();

  SECTION("2. malloc(20) reuses freed block, should split");
  void *b = my_malloc(20);
  assert(a == b);
  print_blocks();

  SECTION("3. malloc(50) reuses the split leftover");
  void *c = my_malloc(50);
  assert(c != NULL);
  print_blocks();

  SECTION("4. free b then c");
  my_free(b);
  my_free(c);
  print_blocks();

  SECTION("5. Chain coalescing across 4 fresh blocks");
  void *p1 = my_malloc(50);
  void *p2 = my_malloc(50);
  void *p3 = my_malloc(50);
  void *p4 = my_malloc(50);
  print_blocks();

  my_free(p3);
  print_blocks();

  my_free(p4);
  print_blocks();

  my_free(p2);
  print_blocks();

  my_free(p1);
  print_blocks();

  SECTION("6. malloc(150) reuses the fully-coalesced block");
  void *big = my_malloc(150);
  assert(big == p1);
  print_blocks();

  SECTION("7. mmap path - large allocation");
  void *m1 = my_malloc(8000);
  assert(m1 != NULL);
  memset(m1, 0xAB, 8000);
  printf("m1=%p\n", m1);
  my_free(m1);
  print_blocks();

  SECTION("8. calloc zero-initializes memory");
  int *arr = (int *)my_calloc(10, sizeof(int));
  assert(arr != NULL);
  for (int i = 0; i < 10; i++)
    assert(arr[i] == 0);
  for (int i = 0; i < 10; i++)
    arr[i] = 99;
  my_free(arr);
  int *arr2 = (int *)my_calloc(10, sizeof(int));
  for (int i = 0; i < 10; i++)
    assert(arr2[i] == 0);
  printf("calloc zero-init: PASS\n");
  my_free(arr2);

  SECTION("9. calloc overflow guard");
  void *overflow = my_calloc((size_t)-1, 2);
  assert(overflow == NULL);
  printf("calloc overflow correctly rejected: PASS\n");

  SECTION("10. realloc grow");
  int *r1 = (int *)my_malloc(5 * sizeof(int));
  for (int i = 0; i < 5; i++)
    r1[i] = i + 1;
  int *r2 = (int *)my_realloc(r1, 20 * sizeof(int));
  assert(r2 != NULL);
  for (int i = 0; i < 5; i++)
    assert(r2[i] == i + 1);
  printf("realloc grow, data preserved: PASS\n");
  print_blocks();

  SECTION("11. realloc shrink");
  int *r3 = (int *)my_realloc(r2, 2 * sizeof(int));
  assert(r3 == r2);
  print_blocks();

  SECTION("12. realloc(NULL, size)");
  void *r4 = my_realloc(NULL, 64);
  assert(r4 != NULL);
  printf("realloc(NULL, size) == malloc: PASS\n");

  SECTION("13. realloc(ptr, 0)");
  void *r5 = my_realloc(r4, 0);
  assert(r5 == NULL);
  printf("realloc(ptr, 0) frees correctly: PASS\n");

  printf("\nAll tests completed without assertion failures.\n");
  return 0;
}
