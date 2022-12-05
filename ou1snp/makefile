CC=gcc
CFLAGS=-Wall -Werror -g -std=gnu11 -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition

mexec: mexec.o
	$(CC) -o mexec mexec.o $(CFLAGS)

mexec.o: mexec.c
	$(CC) -c mexec.c $(CFLAGS)


