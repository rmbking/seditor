#include <stdio.h>
#include <termios.h>
#include "kbhit.h"
#include "view.h"
#include "main.h"
#include "cursor.h"
struct winsize win;
void update_pos(char tmp,int *row,int *col)
{
	switch(tmp){
		case '\n':
			(*row) ++;
			*col = 1;
			break;
		default:
			(*col)++;
	}
}
void next(char c,int *row,int *col)
{
	if(c == '\t') 
		*col = (*col + 3) / TABLEN * TABLEN  + 1;
	else if(c == '\n') {
		cur_state.line_endpos[*row] = *col;
		*col = 1;
		(*row) ++;
	}
	else
		(*col)++;
	if(*col > cur_state.win_width)
	{
		cur_state.line_endpos[*row] = *col - 1;
		*col -= cur_state.win_width;
		(*row) ++;
	}
}
void state_init()
{
	char word;
	int cur_row,cur_col;

	cur_row = 1;
	cur_col = 1;

	fseek(FP,0,SEEK_SET);
	ioctl(STDIN_FILENO,TIOCGWINSZ,&win);
	cur_state.win_height = win.ws_row;
	cur_state.win_width = win.ws_col;

	while((word = fgetc(FP)) != EOF)
		next(word,&cur_row,&cur_col);

	cur_state.total_line =cur_row-1;
}
/*not change about the cur_state content but line_endpos*/
void display(int start_line)
{
	char word;
	int start;
	int row,col;
	int cur_row,cur_col;
	clear_screen();
	fseek(FP,0,SEEK_SET);
	/*
	ioctl(STDIN_FILENO,TIOCGWINSZ,&win);
	cur_state.win_height = win.ws_row;
	cur_state.win_width = win.ws_col;
	*/
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
			start_line = 0;		//fix the bug when the start_line reaches the same line as screen rows.
			cur_row = 1;
			cur_col = 1;
			start = 1;
		}
		if((word = fgetc(FP)) != EOF) 
		{
			next(word,&cur_row,&cur_col);
			if(start)
				if(word != '\t')
					putchar(word);
				else
					CURSOR_MOVE(cur_row,cur_col);
		}
		else if(cur_row < cur_state.win_height && start)
		{
			printf("~\n");
			cur_state.line_endpos[cur_row] = cur_col;
			cur_row++;
		}
		if(cur_row == cur_state.win_height && !start_line)		//add the latter condition to fix the bug as above
			break;
	}
}
int view()
{

	char cmd;
	int row,col;
	state_init();
	/*just for debug
	printf("%d\n",cur_state.total_line);
	sleep(5);
	*/
	display(1);
	CURSOR_MOVE(1,1);
	cur_state.cur_row = 1;
	cur_state.cur_col = 1;
	cur_state.start_line = 1;
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
			case ':':
					 CURSOR_MOVE(cur_state.win_height,1);
					 putchar(':');
					 return CONTROL_MODE;

			case 'h':
					 CursorLeft(1);
					 break;
			case 'j':
					 CursorDown(1);
					 break;
			case 'k':
					 CursorUp(1);
					 break;
			case 'l':
					 CursorRight(1);
					 break;
			case Ctl('f'):
					 
						cur_state.start_line += cur_state.win_height - 2; 
						if(cur_state.start_line > cur_state.total_line - cur_state.win_height + 2) 
							 cur_state.start_line = cur_state.total_line - cur_state.win_height + 2;
						display(cur_state.start_line);
						CURSOR_MOVE(1,1);
						cur_state.cur_row = 1;
					 	
					 /*	//simple implement,but not effecient
						CURSOR_MOVE(cur_state.win_height - 1,1);
						cur_state.cur_row = cur_state.win_height - 1;
						CursorDown(cur_state.win_height - 2);
						CURSOR_MOVE(1,1);
						cur_state.cur_row = 1;
						*/
						break;

			case Ctl('b'):
						cur_state.start_line -= cur_state.win_height - 2;
						if(cur_state.start_line <= 0)
							cur_state.start_line = 1;
						display(cur_state.start_line);
						CURSOR_MOVE(cur_state.win_height-1,1);
						cur_state.cur_row = cur_state.win_height - 1;
						break;
			
			case Ctl('d'):
						cur_state.start_line += cur_state.win_height/2 - 1; 
						if(cur_state.start_line > cur_state.total_line - cur_state.win_height + 2) 
							 cur_state.start_line = cur_state.total_line - cur_state.win_height + 2;
						display(cur_state.start_line);
						CURSOR_MOVE(1,1);
						cur_state.cur_row = 1;
						break;

			case Ctl('u'):
						cur_state.start_line -= cur_state.win_height/2 - 1;
						if(cur_state.start_line <= 0)
							cur_state.start_line = 1;
						display(cur_state.start_line);
						CURSOR_MOVE(cur_state.win_height-1,1);
						cur_state.cur_row = cur_state.win_height - 1;
						break;
			case 'H':
						CURSOR_MOVE(1,1);
						cur_state.cur_row = 1;
						break;
			case 'M':
						CURSOR_MOVE(cur_state.win_height/2,1);
						cur_state.cur_row = cur_state.win_height/2;
						break;
			case 'L':
						CURSOR_MOVE(cur_state.win_height - 1,1);
						cur_state.cur_row = cur_state.win_height - 1;
						break;

			case 's':CursorLocate(&row,&col);printf("%d %d ",row,col);fflush(stdout);	break;
			case 'w':printf("%d %d ",cur_state.win_height,cur_state.win_width);fflush(stdout);break;
			case 't':printf("%d ",cur_state.total_line);fflush(stdout);break;
            default:break;
		}
	}
}
