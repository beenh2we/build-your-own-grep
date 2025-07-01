CC = gcc
CFLAGS = -Wall -Wextra -std=c17

all: grep

grep: src/main.c
	$(CC) $(CFLAGS) -o grep src/main.c

clean:
	rm -f grep
