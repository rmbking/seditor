#include <stdio.h>
#include <termios.h>
#include "kbhit.h"
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
void state_init()
{
	char word;
	int row,col;
	int cur_row,cur_col;
	int line = 1;
	fseek(FP,0,SEEK_SET);
	ioctl(STDIN_FILENO,TIOCGWINSZ,&win);
	row = win.ws_row;
	col = win.ws_col;
	clear_screen();
	CURSOR_MOVE(1,1);
	while((word = fgetc(FP)) != EOF)
	{
		cursor_locate(&cur_row,&cur_col);
		putchar(word);
		if( cur_col == 1)		
			line++;		
	}
	cur_state.total_line = line-1;
	clear_screen();
}
static void display(int start_line)
{
	char word;
	int start;
	int row,col;
	int cur_row,cur_col;
	clear_screen();
	fseek(FP,0,SEEK_SET);
	ioctl(STDIN_FILENO,TIOCGWINSZ,&win);
	row = win.ws_row;
	col = win.ws_col;
	cur_row = 1;
	cur_col = 1;
	start = 0;
	CURSOR_MOVE(1,1);
	while(1)
	{
		word = EOF;
		/*locate the first output line and set out flag*/
		if(cur_row == start_line && start  == 0)
		{
			cur_row = 1;
			cur_col = 1;
			start = 1;
		}
		if(((word = fgetc(FP)) != EOF) && start)
			putchar(word);
		else if(cur_row < row && start)
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
}
int view()
{

	char cmd;
	int row,col;
	state_init();
	display(1);
	CURSOR_MOVE(1,1);
	cur_line = 1;
	cur_pos = 1;
	int start_line = 1;
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

			case 'h':
					 CURSOR_LEFT();
					 if(cur_pos >1)
						 cur_pos--;
					 break;
			case 'j':
					 if(cur_line < win.ws_row - 1)
					 {
						 cur_line ++;
				   	 	CURSOR_DOWN();
					 }
					 else
					 {
						 if(start_line + win.ws_row - 2 < cur_state.total_line )
						 {
							start_line++; 
							display(start_line);
						 }
					 }
					 break;
			case 'k':
					 if(cur_line > 1)
					 {
						 cur_line --;
					 	CURSOR_UP();
					 }
					 else
					 {
						 if(start_line > 1)
						 {
						 	start_line--;
						 	display(start_line);
							CURSOR_MOVE(1,1);
						 }
					 }
					 break;
			case 'l':CURSOR_RIGHT();break;
			case 's':cursor_locate(&row,&col);printf("%d %d ",row,col);fflush(stdout);	break;
			case 'w':printf("%d %d ",win.ws_row,win.ws_col);
            default:break;
		}
	}
}

