CC = gcc
CFLAGS = -Wall -g
LIBS = -lcryptsetup

all: main

main: main.o service.o
	$(CC) $(CFLAGS) -o usblock main.c $(LIBS)
	$(CC) $(CFLAGS) -o service service.c

clean:
	rm -f *.o main service
