#include <stdio.h>	
#include <termios.h>
#include <string.h>
#include "kbhit.h"
#include "view.h"
#include "main.h"
#include "cursor.h"

void next(char c,int index,int *row,int *col,int *row_of_line)
{
	int i;
	int start_col;
	static char change_line = 1;
	if(c == '\t') 
	{
		start_col = *col;	
		*col = (*col + TABLEN - cur_state.start_pos) / TABLEN * TABLEN  + cur_state.start_pos;
		/*to be extended...
		for(i = start_col; i < *col && i <= cur_state.win_width; i++)
			cur_state.map[*row][i] = index;
		******************/
	}
	else if(c == '\n') {
		if(*row < MAX_SCREEN_HEIGHT)	//avoid the access the element of the array out of range(reading large file in state_init).
		{
			cur_state.line_endpos[*row] = *col;
		}
		if(change_line == 1)	//avoid the cursor moving down twice when reading the '\n' at the end of screen row.
		{
//			cur_state.map[*row][*col] = index;
			(*row) ++;
			*col = cur_state.start_pos;
		}
	}
	else
	{
//		cur_state.map[*row][*col] = index;
		(*col)++;
	}
	if(*col > cur_state.win_width)
	{
		if(*row < MAX_SCREEN_HEIGHT)	//avoid the access the element of the array out of range(reading large file in state_init).
			cur_state.line_endpos[*row] = cur_state.win_width;
		*col = *col - cur_state.win_width + cur_state.start_pos - 1;
		change_line = 0;
		(*row) ++;
		(*row_of_line)++;
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
	struct winsize win;
	static base_line = 1024;
	static base_line_width = 256;
	struct file_line char_buf;
	struct file_line *line_buf;
	int line = 1;

	fseek(FP,0,SEEK_SET);
	ioctl(STDIN_FILENO,TIOCGWINSZ,&win);
	cur_state.win_height = win.ws_row;
	cur_state.win_width = win.ws_col;

	//the pointer of the memory storing all the pointers of lines.
	line_buf = (struct file_line *)malloc(base_line * sizeof (struct file_line));
	if(line_buf  == NULL)
	{
		printf("malloc error at function:%s line: %d.\n",__FUNCTION__,__LINE__);
		exit(-1);
	}
	
	//the pointer of the memory storing all the characters of the line.
	char_buf.character = (char *) malloc(base_line_width * sizeof (char));
	if(char_buf.character  == NULL)
	{
		printf("malloc error at function:%s line: %d.\n",__FUNCTION__,__LINE__);
		exit(-1);
	}
	char_buf.character[0] = '+';	//just for debug
	char_buf.line_size = 1;	//times of original size (256) 
	char_buf.line_row = 1;	//columns occupied,modified when displaying if necessary.
	char_buf.line_end = 1;	//index of element waiting to be written.
	
	while(1) 
	{

		word = fgetc(FP);
		if(char_buf.line_end >= lengthof(char_buf))	
		{
			char_buf.line_size *= 2;
			char_buf.character = (char *) realloc(char_buf.character,lengthof(char_buf));
			if(char_buf.character  == NULL)
			{
				printf("realloc error at function:%s line: %d.\n",__FUNCTION__,__LINE__);
				exit(-1);
			}
		}
		writeto(char_buf,word);
		char_buf.line_end++;
		if(word == '\n')	
		{
			char_buf.line_end--;	//line_end points to the '\n'	
			line_buf[line++] = char_buf;
			if(line >= base_line)
			{
				base_line *= 2;
				line_buf = (struct file_line *)realloc(line_buf,base_line * sizeof (struct file_line));
				if(char_buf.character  == NULL)
				{
					printf("realloc error at function:%s line: %d.\n",__FUNCTION__,__LINE__);
					exit(-1);
				}
			}
			char_buf.character = (char *)malloc(base_line_width * sizeof (char));
			if(char_buf.character  == NULL)
			{
				printf("malloc error at function:%s line: %d.\n",__FUNCTION__,__LINE__);
				exit(-1);
			}
			char_buf.character[0] = '+';	//just for debug
			char_buf.line_size = 1;	//times of original size (256) 
			char_buf.line_row = 1;	//columns occupied,modified when displaying if necessary.
			char_buf.line_end = 1;	//index of element waiting to be written.
		}
		if(word == EOF)
		{
			line--;
			break;
		}
	}
	cur_state.line = line_buf;
	cur_state.total_line = line;

	memset(&inbuffer,0,sizeof(inbuffer));
}
int digit_len(int number)
{
	char line_str[10];
	sprintf(line_str,"%d",cur_state.total_line);
	return strlen(line_str);
}
void prepro()	//to be rewritten.....................................
{
	if(cur_state.view_mode & LINESHOW)
	{
		cur_state.start_pos = digit_len(cur_state.total_line) + 2;
	}
	else
		cur_state.start_pos = 1;
	cur_state.cur_col =  cur_state.cur_col + cur_state.start_pos - 1;
}
static int line;
static int pos;
char getc_from_buf()
{
	char word;
	if(line > cur_state.total_line) 
		return EOF;

  	word = cur_state.line[line].character[pos];
	if(cur_state.line[line].character[pos] != '\n')
		pos++;
	else
	{
		line++;
		pos = 1;
	}
	return word;
}
/*not change about the cur_state content but line_endpos and last_row*/

void display(int start_line)
{
	char word;
	int cur_row,cur_col;
	int row_of_line;
	int index;	//store the index of the reading character for .map
	CURSOR_HIDE();
	clear_screen();
	line = start_line;
	pos = 1;
	row_of_line = 1;

	cur_row = 1;
	cur_col = cur_state.start_pos;
	cur_state.last_row = cur_state.win_height - 1;
	index = pos;

	CURSOR_MOVE(1,cur_state.start_pos);
	while(1)
	{
		if((word = getc_from_buf()) != EOF) 
		{
			next(word,index,&cur_row,&cur_col,&row_of_line);

			if(word == '\n')
			{
				cur_state.line[line - 1].line_size = row_of_line;
				row_of_line = 1;
			}
			else if(word != '\t')
				putchar(word);


			CURSOR_MOVE(cur_row,cur_col);
		}
		else if(cur_row < cur_state.win_height )
		{
			if(cur_state.last_row > cur_row - 1)
				cur_state.last_row = cur_row - 1;	//record the previous line of the first ~line
			printf("~\n");
			cur_state.line_endpos[cur_row] = cur_col;
			cur_row++;
		}
		if(cur_row == cur_state.win_height )	
			break;
		index = pos;
	}
	if(cur_state.view_mode & LINESHOW )	//if the number of line needs to be shown
		line_number_list();
	text_info();
	CURSOR_SHOW();
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
		cur_state.cur_line = 1;
		cur_state.cur_index = 1;
		state_init();
		display(1);
		CURSOR_MOVE(1,1);
		CheckCursor();
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
					 if(cur_state.cur_line < cur_state.total_line)
					 {
					 	CursorDown(cur_state.line[cur_state.cur_line].line_size);
					 	cur_state.cur_line ++;
					 }
			 		 clearinbuffer();
					 break;
			case 'k':
					 if(cur_state.cur_line > 1)
					 {
					 	CursorUp(cur_state.line[cur_state.cur_line - 1].line_size);
					 	cur_state.cur_line --;
					 }
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
					//CURSOR_MOVE(cur_state.last_row,cur_state.start_pos);
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
					 printf("%d %d",cur_state.cur_line,cur_state.cur_index);
					 fflush(stdout);
					 /*
					 for(i = 1 ; i < cur_state.line_endpos[cur_state.cur_row - 1]; i++)
						 if(cur_state.character[cur_state.cur_row-1][i] == '\t')
							 putchar('t');
						 else 
						 if(cur_state.character[cur_state.cur_row-1][i] == '\n')
							 putchar('n');
						 else 
						 putchar(cur_state.character[cur_state.cur_row-1][i]);
						 */
					 break;
			/*for debug*/
            default:
					 addinbuffer(cmd);
					 break;
		}
		text_info();
	}
}
