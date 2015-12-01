#include <stdio.h>
#include <termios.h>
#include "view.h"
#include "main.h"
#include "cursor.h"
int  cur_line;
int  cur_pos; 
int  total_line;
int  char_num_of_line[100];
struct winsize win;
void update_pos(char tmp,int *row,int *col)
{
	switch(tmp){
		case '\n':
			char_num_of_line[*row] = *col;
			(*row) ++;
			*col = 1;
			break;
		default:
			(*col)++;
	}
}
static void display()
{
	char word;
	int row,col;
	int cur_row,cur_col;
	ioctl(STDIN_FILENO,TIOCGWINSZ,&win);
	row = win.ws_row;
	col = win.ws_col;
	cur_row = 1;
	cur_col = 0;
	while(1)
	{
		word = EOF;
		if((word = fgetc(FP)) != EOF)
			putchar(word);
		else if(cur_row < row)
		{
			printf("~\n");
			cur_row++;
		}
		update_pos(word,&cur_row,&cur_col);
		if(cur_col > col )
		{
			char_num_of_line[cur_row] = cur_col - 1;
			cur_col = 1;
			cur_row++;
		}
		if(cur_row == row)
			break;
	}
	CURSOR_MOVE(1,1);
}
int view()
{

	char cmd;
	display();
    while(cmd = kb_input())
    {
    	if(cmd == FAILURE)
        {
        	printf("Something is wrong the keyboard.\n");
            exit(2);
        }
        switch(cmd)
        {
        	case 'i':return EDIT_MODE;

			case 'h':CURSOR_LEFT();break;
			case 'j':CURSOR_DOWN();break;
			case 'k':CURSOR_UP();break;
			case 'l':CURSOR_RIGHT();break;
            default:break;
		}
	}
}

