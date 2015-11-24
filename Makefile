CC=gcc
all:*.c *.h
	mkdir tmp
	$(CC) -c *.c -o *.o
	$(CC) *.o -o seditor
clean:
	rm *.o
	rm seditor
