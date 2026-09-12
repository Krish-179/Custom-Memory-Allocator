#include <stdio.h>
#include <sys/mman.h>
#include <unistd.h>
#define CHUNK_SIZE (4 * 1024)
#define USED 0
#define FREE 1
#define MMAPPED 2

typedef struct {
  size_t prev_size;
  size_t size;
  int flag;
} header;

static header *start = NULL;
static void *end = NULL;

void *find_free_block(size_t size) {
  header *curr = NULL;
  header *next_block = NULL;
  for (next_block = start; (void *)next_block != end;
       next_block =
           (header *)((char *)next_block + sizeof(header) + next_block->size)) {
    if (next_block->flag == FREE && next_block->size >= size) {
      next_block->prev_size = (curr != NULL) ? curr->size : 0;
      return next_block;
    }
    curr = next_block;
  }
  return curr;
}

void *my_malloc(size_t size) {

  if (size > CHUNK_SIZE) {
    void *chunk = mmap(NULL, sizeof(header) + size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

    if (chunk == (void *)-1) {
      printf("Error: Getting chunk from mmap");
      return NULL;
    } else {
      header *head = (header *)chunk;
      head->size = size;
      head->flag = MMAPPED;
      return (void *)(head + 1);
    }
  }
  header *free_block = NULL;
  if (start) {
    free_block = find_free_block(size);
    if (free_block->flag == FREE && free_block->size >= size) {
      if ((free_block->size - size) > sizeof(header)) {
        header *block = (header *)((char *)free_block + sizeof(header) + size);
        block->flag = FREE;
        block->size = free_block->size - size - sizeof(header);
        block->prev_size = size;
        free_block->size = size;
	header *after = (header *)((char *)block + sizeof(header) + block->size);
	if ((void *)after != end){
		after->prev_size = block->size;
	}
      }
      free_block->flag = USED;
      return (void *)(free_block + 1);
    }
  }
  size_t needed = size + 2 * (sizeof(header));
  size_t chunk_size = needed > CHUNK_SIZE ? needed : CHUNK_SIZE;
  void *chunk = sbrk(chunk_size);
  if (chunk == (void *)-1) {
    printf("Error: Getting chuck from sbrk\n");
    return NULL;
  }
  header *head = (header *)chunk;
  size_t leftover = chunk_size - size - sizeof(header);
  head->prev_size = free_block ? free_block->size : 0;
  head->flag = USED;
  if (!start) {
    start = head;
  }
  if (leftover > sizeof(header)) {
    head->size = size;
    header *next_block = (header *)((char *)head + sizeof(header) + size);
    next_block->flag = FREE;
    next_block->prev_size = size;
    next_block->size = leftover - sizeof(header);
  } else {
    head->size = chunk_size - sizeof(header);
  }
  end = (char *)head + chunk_size;
  return (void *)(head + 1);
}

void my_free(void *ptr) {
  if (!ptr) {
    printf("Error: Can't Free Chuck\n");
    return;
  }
  header *h = (header *)((char *)ptr - sizeof(header));
  if (h->flag == MMAPPED) {
    munmap(h, h->size + sizeof(header));
    return;
  }
  header *next_block = (header *)((char *)ptr + h->size);
  if ((void *)next_block != end && next_block->flag == FREE) {
    h->size += sizeof(header) + next_block->size;
  }
  if (h != start) {
    header *prev_block = (header *)((char *)h - sizeof(header) - h->prev_size);
    if (prev_block->flag == FREE) {
      prev_block->size += sizeof(header) + h->size;
      prev_block->flag = FREE;
    }
  }

  h->flag = FREE;
}
void print_blocks(void) {
  header *curr = start;
  int i = 0;
  while ((void *)curr != end) {
    printf("block %d: addr=%p size=%zu prev_size=%zu flag=%s\n", i++,
           (void *)curr, curr->size, curr->prev_size,
           curr->flag == USED ? "USED"
                              : (curr->flag == FREE ? "FREE" : "MMAPPED"));
    curr = (header *)((char *)curr + sizeof(header) + curr->size);
  }
  printf("total blocks: %d\n", i);
}
