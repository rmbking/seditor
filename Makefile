CC=gcc
OBJ = main.o kbhit.o view.o	edit.o control.o
HEAD = kbhit.h main.h cursor.h	edit.h control.h
all: $(OBJ)
	$(CC) -o seditor -g $(OBJ)
	cp seditor ~/bin/
%.o:%.c $(HEAD) 
	$(CC) -o  $@ -c $< -g	
clean:
	-rm *.o
	-rm seditor

