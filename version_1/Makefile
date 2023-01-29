CC = gcc
CFLAGS = -Wall -Wextra -g

all: test

test: test.o malloc.o
test.o: test.c malloc.h
malloc.o: malloc.c malloc.h

.PHONY: clean

clean:
	${RM} test test.o malloc.o
