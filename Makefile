CC=gcc
OBJ = main.o kbhit.o view.o
HEAD = kbhit.h seditor.h
all: $(OBJ)
	$(CC) -o seditor $(OBJ)
%.o:%.c $(HEAD) 
	$(CC) -o  $@ -c $<
clean:
	rm *.o
	rm seditor

