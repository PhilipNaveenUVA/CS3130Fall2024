# Makefile

CFLAGS = -D_XOPEN_SOURCE=700 -Og -g -std=c11 -Wall -pedantic -fsanitize=address
LDFLAGS = -fsanitize=address

all: split

split: main.o split.o
	$(CC) $(CFLAGS) -o split main.o split.o $(LDFLAGS)

main.o: main.c split.h
	$(CC) $(CFLAGS) -c main.c

split.o: split.c split.h
	$(CC) $(CFLAGS) -c split.c

clean:
	rm -f split main.o split.o

