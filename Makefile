CC=gcc
OBJ = main.o kbhit.o view.o
HEAD = kbhit.h main.h cursor.h
all: $(OBJ)
	$(CC) -o seditor -g $(OBJ)
	cp seditor ~/bin/
%.o:%.c $(HEAD) 
	$(CC) -o  $@ -c $< -g	
clean:
	-rm *.o
	-rm seditor

