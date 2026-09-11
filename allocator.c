#include <stdio.h>
#include <unistd.h>
#define CHUNK_SIZE (4 * 1024)
#define USED 0
#define FREE 1
#define MMAPPED 2

typedef struct {
  size_t size;
  int flag;
} header;

static header *start = NULL;
static void *end = NULL;

void *find_free_block(size_t size) {
  header *curr = NULL;
  for (curr = start; (void *)curr != end;
       curr = (header *)((char *)curr + sizeof(header) + curr->size)) {
    if (curr->flag == FREE && curr->size >= size) {
      return curr;
    }
  }
  return NULL;
}

void *my_malloc(size_t size) {
  header *free_block = find_free_block(size);
  if (free_block) {
    if ((free_block->size - size) > sizeof(header)) {
      header *block = (header *)((char *)free_block + sizeof(header) + size);
      block->flag = FREE;
      block->size = free_block->size - size - sizeof(header);
      free_block->size = size;
    }
    free_block->flag = USED;
    return (void *)(free_block + 1);
  }
  void *chunk = sbrk(CHUNK_SIZE);
  if (chunk == (void *)-1) {
    printf("Error: Getting chuck from sbrk\n");
    return NULL;
  }
  header *head = (header *)chunk;
  head->size = size;
  head->flag = USED;
  if (!start) {
    start = head;
  }
  end = (char *)head + CHUNK_SIZE;
  return (void *)(head + 1);
}

void my_free(void *ptr) {
  if (!ptr) {
    printf("Error: Can't Free Chuck\n");
    return;
  }
  header *h = (header *)((char *)ptr - sizeof(header));
  h->flag = FREE;
}

void print_blocks(void) {
  header *curr = start;
  int i = 0;
  while ((void *)curr != end) {
    printf("block %d: addr=%p size=%zu flag=%s\n", i++, (void *)curr,
           curr->size, curr->flag == USED ? "USED" : "FREE");
    curr = (header *)((char *)curr + sizeof(header) + curr->size);
  }
}
