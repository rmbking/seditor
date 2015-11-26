CC=gcc
all: *.h *.c
	$(CC) -o view.o -c view.c
	$(CC) -o kbhit.o -c kbhit.c
	$(CC) -o seditor view.o	kbhit.o
clean:
	rm *.o
	rm seditor

