C = clang
CFLAGS = -Wall -Wextra -g

all: test

test: main.c allocator.c allocator.h
		$(CC) $(CFLAGS) -o test main.c allocator.c

clean:
		rm -f test
