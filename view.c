#include <stdio.h>
#include <termios.h>
#include <string.h>
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

	memset(&inbuffer,0,sizeof(inbuffer));
}
int digit_len(int number)
{
	char line_str[10];
	sprintf(line_str,"%d",cur_state.total_line);
	return strlen(line_str);
}
void prepro()
{
	if(cur_state.view_mode & LINESHOW)
	{
		cur_state.start_pos = digit_len(cur_state.total_line) + 1;
	}
	else
		cur_state.start_pos = 0;

}
/*not change about the cur_state content but line_endpos*/
void display(int start_line)
{
	char word;
	int start;
	int row,col;
	int cur_row,cur_col;
	int tmp;

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
			{
				if(word == '\n')
					putchar(word);
				/*
				if(cur_state.cur_col == 1)
				{
					tmp = digit_len(cur_row);
					tmp = cur_state.start_pos - tmp;
					CURSOR_MOVE(cur_row,tmp);
					printf("%d ",cur_row);
				}
				*/
				if(word != '\t' && word != '\n')
					putchar(word);
				else
					CURSOR_MOVE(cur_row,cur_col);
			}
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
	text_info();
}
int view()
{

	char cmd;
	int row,col;
	if(cur_state.view_mode == 0)
	{
	/*just for debug
	printf("%d\n",cur_state.total_line);
	sleep(5);
	*/
		state_init();
		display(1);
		CURSOR_MOVE(1,1);
		cur_state.cur_row = 1;
		cur_state.cur_col = 1;
		cur_state.start_line = 1;
		cur_state.cur_pos = 1;
	}
	else
		prepro();
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
			 		 clearinbuffer();
					 return CONTROL_MODE;

			case 'h':
					 CursorLeft(1);
			 		 clearinbuffer();
					 break;
			case 'j':
					 CursorDown(1);
			 		 clearinbuffer();
					 break;
			case 'k':
					 CursorUp(1);
			 		 clearinbuffer();
					 break;
			case 'l':
					 CursorRight(1);
			 		 clearinbuffer();
					 break;
			case 'g':
					if(inbuffer.buf[inbuffer.size-1] == 'g')
					{
						display(1);
						cur_state.start_line = 1;
						cur_state.cur_row = 1;
						cur_state.cur_col = 1;
						CURSOR_MOVE(1,1);
						clearinbuffer();
						break;
					}
					addinbuffer('g');
					break;
			case 'G':
					display(cur_state.total_line - cur_state.win_height + 2);
					cur_state.start_line = cur_state.total_line - cur_state.win_height + 2;
					cur_state.cur_row = cur_state.win_height - 1;
					cur_state.cur_col = 1;
					CURSOR_MOVE(cur_state.win_height-1,1);
			 		 clearinbuffer();
					break;
			case Ctl('f'):
					 
						cur_state.start_line += cur_state.win_height - 2; 
						if(cur_state.start_line > cur_state.total_line - cur_state.win_height + 2) 
							 cur_state.start_line = cur_state.total_line - cur_state.win_height + 2;
						display(cur_state.start_line);
						CURSOR_MOVE(1,cur_state.cur_pos);
						cur_state.cur_row = 1;
						cur_state.cur_col = cur_state.cur_pos;
						CheckCursor();
					 	
					 /*	//simple implement,but not effecient
						CURSOR_MOVE(cur_state.win_height - 1,1);
						cur_state.cur_row = cur_state.win_height - 1;
						CursorDown(cur_state.win_height - 2);
						CURSOR_MOVE(1,1);
						cur_state.cur_row = 1;
						*/
			 		 clearinbuffer();
						break;

			case Ctl('b'):
						cur_state.start_line -= cur_state.win_height - 2;
						if(cur_state.start_line <= 0)
							cur_state.start_line = 1;
						display(cur_state.start_line);
						CURSOR_MOVE(cur_state.win_height-1,cur_state.cur_pos);
						cur_state.cur_row = cur_state.win_height - 1;
						cur_state.cur_col = cur_state.cur_pos;
						CheckCursor();
			 		 clearinbuffer();
						break;
			
			case Ctl('d'):
						cur_state.start_line += cur_state.win_height/2 - 1; 
						if(cur_state.start_line > cur_state.total_line - cur_state.win_height + 2) 
							 cur_state.start_line = cur_state.total_line - cur_state.win_height + 2;
						display(cur_state.start_line);
						CURSOR_MOVE(1,cur_state.cur_pos);
						cur_state.cur_row = 1;
						cur_state.cur_col = cur_state.cur_pos;
						CheckCursor();
			 		 clearinbuffer();
						break;

			case Ctl('u'):
						cur_state.start_line -= cur_state.win_height/2 - 1;
						if(cur_state.start_line <= 0)
							cur_state.start_line = 1;
						display(cur_state.start_line);
						CURSOR_MOVE(cur_state.win_height-1,cur_state.cur_pos);
						cur_state.cur_row = cur_state.win_height - 1;
						cur_state.cur_col = cur_state.cur_pos;
						CheckCursor();
			 		 clearinbuffer();
						break;
			case 'H':
						CURSOR_MOVE(1,1);
						cur_state.cur_row = 1;
			 		 clearinbuffer();
						break;
			case 'M':
						CURSOR_MOVE(cur_state.win_height/2,1);
						cur_state.cur_row = cur_state.win_height/2;
			 		 clearinbuffer();
						break;
			case 'L':
						CURSOR_MOVE(cur_state.win_height - 1,1);
						cur_state.cur_row = cur_state.win_height - 1;
			 		 clearinbuffer();
						break;

			case 's':CursorLocate(&row,&col);printf("%d %d ",row,col);fflush(stdout);	break;
			case 'w':printf("%d %d ",cur_state.win_height,cur_state.win_width);fflush(stdout);break;
			case 't':printf("%d ",cur_state.total_line);fflush(stdout);break;
			case 'd':printf("%d",cur_state.start_pos);break;
            default:
					 addinbuffer(cmd);
					 break;
		}
		text_info();
	}
}
