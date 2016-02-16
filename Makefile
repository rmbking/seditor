CC=gcc
OBJ = main.o kbhit.o cursor.o view.o edit.o control.o util.o
HEAD = kbhit.h main.h cursor.h	edit.h control.h util.h
all: $(OBJ)
	$(CC) -o seditor -g $(OBJ)
	cp seditor ~/bin/
%.o:%.c $(HEAD) 
	$(CC) -o  $@ -c $< -g	
clean:
	-rm *.o
	-rm seditor

