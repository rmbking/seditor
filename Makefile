CC=gcc
all: *.h *.c
	$(CC) -o view.o -c view.c
	$(CC) -o seditor view.o
clean:
	rm *.o
	rm seditor

