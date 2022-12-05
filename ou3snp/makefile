CC=gcc
CFLAGS= -Wall -Werror -g -std=gnu11 -lpthread -Wextra -Wpedantic -Wmissing-declarations -Wmissing-prototypes -Wold-style-definition

mdu: mdu.o stack.o
	$(CC) $(CFLAGS) -pthread -o mdu mdu.o stack.o
mdu.o: mdu.c stack.h
	$(CC) $(CFLAGS) -c mdu.c 

stack.o: stack.c stack.h
	$(CC) $(CFLAGS) -c stack.c

