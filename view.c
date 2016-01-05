#include <stdio.h>	
#include <termios.h>
#include <string.h>
#include "kbhit.h"
#include "view.h"
#include "main.h"
#include "cursor.h"

void next(char c,int *row,int *col)
{
	int i;
	int tmp_row,tmp_col;
	static char change_line = 1;
	if(c == '\t') 
	{
		tmp_col = *col;	
		*col = (*col + TABLEN - cur_state.start_pos) / TABLEN * TABLEN  + cur_state.start_pos;
		if(*row <= MAX_SCREEN_HEIGHT)	//avoid the access the element of the array out of range(reading large file in state_init).
		{	
			for(i = tmp_col; i < *col && i <= cur_state.win_width; i++)
				cur_state.character[*row][i] = '\t';
		}
	}
	else if(c == '\n') {
		if(*row <= MAX_SCREEN_HEIGHT)	//avoid the access the element of the array out of range(reading large file in state_init).
		{
			cur_state.line_endpos[*row] = *col;
			cur_state.character[*row][*col] = '\n';
		}
		if(change_line == 1)	//avoid the cursor moving down twice when reading the '\n' at the end of screen row.
		{
			(*row) ++;
			*col = cur_state.start_pos;
		}
	}
	else
	{
		if(*row <= MAX_SCREEN_HEIGHT)	//avoid the access the element of the array out of range(reading large file in state_init).
			cur_state.character[*row][*col] = c;
		(*col)++;
	}
	if(*col > cur_state.win_width)
	{
		if(*row <= MAX_SCREEN_HEIGHT)	//avoid the access the element of the array out of range(reading large file in state_init).
			cur_state.line_endpos[*row] = cur_state.win_width;
		*col = *col - cur_state.win_width + cur_state.start_pos - 1;
		change_line = 0;
		(*row) ++;
	}
	else 
		change_line = 1;	
}
void line_number_list()
{
	char format[100];
	int line_no = cur_state.start_line;
	int cur_row = 1;
	sprintf(format,"%%%dd ",cur_state.start_pos-2);	//a blank space follows the number
	while(cur_row <= cur_state.win_height-1 && line_no <= cur_state.total_line)
	{
		CURSOR_MOVE(cur_row,1);
		printf(format,line_no);
		cur_row++;
		line_no++;
	}
	
}
void state_init()
{
	char word;
	int cur_row,cur_col;
	int tmp;
	struct winsize win;

	cur_row = 1;
	cur_col = cur_state.start_pos;

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
		cur_state.start_pos = digit_len(cur_state.total_line) + 2;
	}
	else
		cur_state.start_pos = 1;
	cur_state.cur_col =  cur_state.cur_col + cur_state.start_pos - 1;
}
/*not change about the cur_state content but line_endpos and last_row*/

void display(int start_line)
{
	char word;
	int start;	//start flag to print
	int end;	//end flag to print
	int cur_row,cur_col;
	char tmp_str[100];

	clear_screen();
	fseek(FP,0,SEEK_SET);

	cur_row = 1;
	cur_col = 1;
	start = 0;
	end = 0;
	cur_state.last_row = cur_state.win_height - 1;

	CURSOR_MOVE(1,cur_state.start_pos);
	while(1)
	{
		word = EOF;
		/*locate the first output line and set out flag*/
		if(cur_row == start_line && start  == 0)
		{
			start = 1;
			end = 1;		//fix the bug when the start_line reaches the same line as screen rows.
			cur_row = 1;
			cur_col = cur_state.start_pos;
		}
		if((word = fgetc(FP)) != EOF) 
		{
			next(word,&cur_row,&cur_col);
			if(start)
			{
				if(word != '\t' && word != '\n')
					putchar(word);
//				else	//to prove the long line jumps to the proper position of next line.
					CURSOR_MOVE(cur_row,cur_col);
			}
		}
		else if(cur_row < cur_state.win_height && start)
		{
			if(cur_state.last_row > cur_row - 1)
				cur_state.last_row = cur_row - 1;	//record the previous line of the first ~line
			printf("~\n");
			cur_state.line_endpos[cur_row] = cur_col;
			cur_row++;
		}
		if(cur_row == cur_state.win_height && end)		//add the latter condition to fix the bug as above
			break;
	}
	if(cur_state.view_mode & LINESHOW )	//if the number of line needs to be shown
		line_number_list();
	text_info();
}
int view()
{

	char cmd;
	int row,col;
	static int flag = 0;
	static int save_mode = 0;
	int i;
	int modified_mode = save_mode ^ cur_state.view_mode;
	save_mode = cur_state.view_mode;
	if(flag == 0)
	{
	/*just for debug
	printf("%d\n",cur_state.total_line);
	sleep(5);
	*/
		flag = 1;
		cur_state.cur_row = 1;
		cur_state.cur_col = 1;
		cur_state.start_line = 1;
		cur_state.cur_pos = 1;
		state_init();
		display(1);
		CURSOR_MOVE(1,1);
	}
	else if (modified_mode & LINESHOW)
	{
		if(cur_state.view_mode & LINESHOW)
		{
			prepro();
			state_init();
			display(cur_state.start_line);
			CheckCursor();
		}
		else
		{
			cur_state.cur_col = cur_state.cur_col - cur_state.start_pos + 1;
			cur_state.start_pos = 1;
			state_init();
			display(cur_state.start_line);
			CheckCursor();
		}
	}
	
	text_info();	//for moving the cursor to its right position immeditely.
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
						cur_state.start_line = 1;
						display(1);
						cur_state.cur_row = 1;
						cur_state.cur_col = cur_state.start_pos;
						CURSOR_MOVE(1,cur_state.start_pos);
						clearinbuffer();
						break;
					}
					addinbuffer('g');
					break;
			case 'G':
					if(cur_state.total_line > cur_state.win_height - 2)
					{
						cur_state.start_line = cur_state.total_line - cur_state.win_height + 2;
						display(cur_state.total_line - cur_state.win_height + 2);
					}
					else
					{
						display(1);
						cur_state.start_line = 1;
					}
					cur_state.cur_row = cur_state.last_row;
					cur_state.cur_col = cur_state.start_pos;
					CURSOR_MOVE(cur_state.last_row,cur_state.start_pos);
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
						CURSOR_MOVE(1,cur_state.start_pos);
						cur_state.cur_row = 1;
						cur_state.cur_col = cur_state.start_pos;
			 		 clearinbuffer();
						break;
			case 'M':
						CURSOR_MOVE(cur_state.win_height/2,cur_state.start_pos);
						cur_state.cur_row = cur_state.win_height/2;
						cur_state.cur_col = cur_state.start_pos;
						CheckCursor();
			 		 clearinbuffer();
						break;
			case 'L':
						CURSOR_MOVE(cur_state.win_height - 1,cur_state.start_pos);
						cur_state.cur_row = cur_state.win_height - 1;
						cur_state.cur_col = cur_state.start_pos;
						CheckCursor();
			 		 clearinbuffer();
						break;

			/*for debug*/
			case 's':CursorLocate(&row,&col);printf("%d %d ",row,col);fflush(stdout);	break;
			case 'w':printf("%d %d ",cur_state.win_height,cur_state.win_width);fflush(stdout);break;
			case 't':printf("%d ",cur_state.total_line);fflush(stdout);break;
			case 'd':printf("%d",cur_state.start_pos);break;
			case 'e':printf("%d",cur_state.line_endpos[cur_state.cur_row]);break;
			case 'c':
					 for(i = 1 ; i < cur_state.line_endpos[cur_state.cur_row - 1]; i++)
						 if(cur_state.character[cur_state.cur_row-1][i] == '\t')
							 putchar('t');
						 else 
						 if(cur_state.character[cur_state.cur_row-1][i] == '\n')
							 putchar('n');
						 else 
						 putchar(cur_state.character[cur_state.cur_row-1][i]);
					 break;
			/*for debug*/
            default:
					 addinbuffer(cmd);
					 break;
		}
		text_info();
	}
}
