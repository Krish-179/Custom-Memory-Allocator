USTOM MEMORY ALLOCATOR (C)

A from-scratch implementation of malloc, free, calloc, and realloc in C, built to understand how real allocators (glibc's ptmalloc2, the Linux kernel's kmalloc/slab) manage memory internally.

DESIGN

Implicit free list — all blocks (used and free) sit contiguously in memory. There are no next/prev pointers; traversal works by reading each block's size field and jumping forward by sizeof(header) + size.

Header struct (16-24 bytes depending on padding):

typedef struct {
    size_t prev_size;
        size_t size;
            int flag;
            } header;

            prev_size holds the size of the immediately preceding block, used for backward coalescing. flag is USED, FREE, or MMAPPED.

            Splitting — when a free block is larger than a request, it's carved into a used block and a new, independently-reusable free block, provided the leftover is big enough to hold another header.

            Bidirectional coalescing — on free(), both the forward neighbor (via size) and the backward neighbor (via prev_size) are checked and merged if free. Single-step only, no cascading past one neighbor in each direction.

            mmap threshold — requests larger than CHUNK_SIZE bypass the heap entirely and go straight to mmap/munmap. These blocks are never split, coalesced, or reused; each request gets a fresh mapping, freed immediately on free().

            FILES

            allocator.h / allocator.c — the allocator itself (my_malloc, my_free, my_calloc, my_realloc, print_blocks)

            main.c — test suite exercising reuse, splitting, forward/backward/chained coalescing, the mmap path, calloc zero-init and overflow rejection, and all four realloc cases (grow, shrink, NULL, zero)

            Makefile — builds the project

            BUILD AND RUN

            make
            ./test

            Adjust ./test if your Makefile names the output binary something else.

            API

            void *my_malloc(size_t size);
            void my_free(void *ptr);
            void *my_calloc(size_t n_elements, size_t size);
            void *my_realloc(void *ptr, size_t size);

            KNOWN LIMITATIONS (deliberate, not oversights)

            No explicit free list — search is O(n) over all blocks in the heap, not just free ones.

            Coalescing is single-step, not cascading — a free() call merges with at most one neighbor in each direction.

            No thread safety — no locking around global heap state. Single-threaded use only.
