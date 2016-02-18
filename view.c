#include <stdio.h>	
#include <termios.h>
#include <string.h>
#include "kbhit.h"
#include "view.h"
#include "main.h"
#include "cursor.h"
static int line;
static int pos;

void getpos()
{
	file.cur_line = screen.line[screen.cur_row];
	file.cur_index = screen.map[screen.cur_row][screen.cur_col];
}
void next(char c,int index,int *row,int *col,int *row_of_line)
{
	int i;
	int start_col;
	static char change_line = 1;
	if(c == '\t') 
	{
		start_col = *col;	
		*col = (*col + TABLEN - screen.start_pos) / TABLEN * TABLEN  + screen.start_pos;
		for(i = start_col; i < *col && i <= screen.win_width; i++)
			screen.map[*row][i] = index;
	}
	else if(c == '\n') {
		screen.line[*row] = line-1; 	//the getc_from_buf has increase the line
		if(*row < MAX_SCREEN_HEIGHT)	//avoid the access the element of the array out of range(reading large file in state_init).
		{
			screen.row_end[*row] = *col;
		}
		screen.row_rank[*row] = *row_of_line;
		if(change_line == 1)	//avoid the cursor moving down twice when reading the '\n' at the end of screen row.
		{
			screen.map[*row][*col] = index;
			(*row) ++;
			*col = screen.start_pos;
		}
		else
			(*row_of_line)--;
	}
	else
	{
		screen.map[*row][*col] = index;
		(*col)++;
	}
	if(*col > screen.win_width)
	{
		if(*row < MAX_SCREEN_HEIGHT)	//avoid the access the element of the array out of range(reading large file in state_init).
			screen.row_end[*row] = screen.win_width;
		screen.row_rank[*row] = *row_of_line;
		screen.line[*row] = line; 
		*col = *col - screen.win_width + screen.start_pos - 1;
		change_line = 0;
		(*row) ++;
		(*row_of_line)++;
	}
	else 
		change_line = 1;	
}
void line_number_list(int flag)
{
	char format[100];
	int line_no = file.start_line;
	int cur_row = 1;
	sprintf(format,"%%%dd ",screen.start_pos-2);	//a blank space follows the number
	while(cur_row <= screen.win_height-1 && line_no <= file.total_line)
	{
		CURSOR_MOVE(cur_row,1);
		if(flag)
			printf(format,line_no);
		cur_row += file.line[line_no].line_row;
		line_no++;
	}
	file.last_line = line_no-1;
	
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
	screen.win_height = win.ws_row;
	screen.win_width = win.ws_col;

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
	file.line = line_buf;
	file.total_line = line;

	memset(&inbuffer,0,sizeof(inbuffer));
}
int digit_len(int number)
{
	char line_str[10];
	sprintf(line_str,"%d",file.total_line);
	return strlen(line_str);
}
void prepro()	//to be rewritten.....................................
{
	if(screen.view_mode & LINESHOW)
	{
		screen.start_pos = digit_len(file.total_line) + 2;
	}
	else
		screen.start_pos = 1;
	screen.cur_col =  screen.cur_col + screen.start_pos - 1;
}
char getc_from_buf()
{
	char word;
	if(line > file.total_line) 
		return EOF;

  	word = file.line[line].character[pos];
	if(file.line[line].character[pos] != '\n')
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
	int end_flag;
	int index;	//store the index of the reading character for .map
	CURSOR_HIDE();
	clear_screen();
	line = start_line;
	pos = 1;
	row_of_line = 1;
	end_flag = 0;

	cur_row = 1;
	cur_col = screen.start_pos;
	screen.last_row = screen.win_height - 1;
	index = pos;

	CURSOR_MOVE(1,screen.start_pos);
	while(1)
	{
		word = getc_from_buf();
		if(word != EOF) 
		{
			next(word,index,&cur_row,&cur_col,&row_of_line);

			if(word == '\n')
			{
				file.line[line - 1].line_row = row_of_line;
				row_of_line = 1;
				if(end_flag == 1)
					break;
				else if(end_flag == 2)
					end_flag--;
			}
			else if(word != '\t' && !end_flag)
				putchar(word);

			if(!end_flag)
				CURSOR_MOVE(cur_row,cur_col);
		}
		else if(cur_row < screen.win_height )
		{
			if(screen.last_row > cur_row - 1)
				screen.last_row = cur_row - 1;	//record the previous line of the first ~line
			CURSOR_MOVE(cur_row,1);
			printf("~\n");
			screen.row_end[cur_row] = cur_col;
			cur_row++;
		}
		else
			break;
		if(cur_row == screen.win_height )	
		{
		/*preread one line unless getting the '\n' for moving down
			if(word == '\n')
				break;
			else
		*/
			if(word == '\n')
				end_flag = 1;	//preread one line afterwards.
			else
				end_flag = 2;	//finishing this line and then preread one line.
		}
		
		index = pos;
	}
	if(screen.view_mode & LINESHOW )	//if the number of line needs to be shown
		line_number_list(1);
	else
		line_number_list(0);
	CURSOR_SHOW();
	getpos();
	text_info();
}
void line_down()
{
	CursorDown(1);
	clearinbuffer();
	getpos();
}
void line_up()
{
	CursorUp(1);
	clearinbuffer();
	getpos();
}
void word_left()
{
	CursorLeft(1);
	clearinbuffer();
	getpos();
}
void word_right()
{
	CursorRight(1);
	clearinbuffer();
	getpos();
}
void page_down()
{
	file.start_line = file.last_line; 
	display(file.start_line);
	screen.cur_row = 1;
	screen.cur_col = screen.start_pos;
	CursorMove();
	CheckCursor();
	getpos();
 	clearinbuffer();
}
void page_up()
{
	int tmp;
	tmp = file.start_line;
	if(tmp == 1)
	{
		screen.cur_row = 1;
		screen.cur_col = screen.start_pos;
		CursorMove();
		CheckCursor();
		getpos();
		return;
	}
	file.start_line -= screen.win_height - 1;
	if(file.start_line <= 0)
		file.start_line = 1;
	display(file.start_line);
	while(file.last_line < tmp)
	{
		file.start_line++;
		display(file.start_line);
	}
	screen.cur_row = screen.win_height - 1;
	screen.cur_col = screen.start_pos;
	CursorMove();
	CheckCursor();
	getpos();
	clearinbuffer();
}
void half_page_up()
{
	int tmp;
	tmp = screen.win_height/2 - 1;
	file.start_line -= tmp;
	if(file.start_line <= 1)
	{
		file.start_line = 1;
		display(file.start_line);
		screen.cur_row = 1;
		screen.cur_col = screen.start_pos;
		CursorMove();
		CheckCursor();
		getpos();
		return;
	}
	if(tmp > screen.last_row)
		tmp = screen.last_row;
	tmp = screen.line[tmp];
	display(file.start_line);
	while(file.last_line < tmp)
	{
		file.start_line++;
		display(file.start_line);
	}
	screen.cur_row = screen.last_row;
	screen.cur_col = screen.start_pos;
	CursorMove();
	CheckCursor();
	getpos();
					/*
					file.start_line -= screen.win_height/2 - 1;
					if(file.start_line <= 0)
						file.start_line = 1;
					display(file.start_line);
					screen.cur_row = screen.win_height - 1;
					screen.cur_col = screen.col_offset;
					CursorMove();
					CheckCursor();
					getpos();
		 		 	clearinbuffer();
					*/
}
void half_page_down()
{
	int tmp;

	tmp = screen.win_height/2 - 1;
	if(screen.last_row < tmp)
	{
		file.start_line = file.total_line;
		display(file.start_line);
		screen.cur_row = 1;
		screen.cur_col = screen.start_pos;
		CursorMove();
		CheckCursor();
		getpos();
		return;
	}
	tmp = screen.line[tmp];
	file.start_line = tmp;
	display(file.start_line);
	screen.cur_row = 1;
	screen.cur_col = screen.start_pos;
	CursorMove();
	CheckCursor();
	getpos();
	/*
	file.start_line += screen.win_height/2 - 1; 
	if(file.start_line > file.total_line - screen.win_height + 2) 
			 file.start_line = file.total_line - screen.win_height + 2;
		display(file.start_line);
		CURSOR_MOVE(1,screen.col_offset);
		screen.cur_row = 1;
		screen.cur_col = screen.col_offset;
		CheckCursor();
			 			clearinbuffer();
	*/
}
void move_to_line_head()
{
	int tmp;

	tmp = screen.cur_row;
	tmp = screen.row_rank[tmp] - 1;
	screen.cur_row -= tmp;	
	screen.cur_col = screen.start_pos;
	CursorMove();
	CheckCursor();
	getpos();
}
void move_to_line_tail()
{
	int tmp;

	tmp = screen.cur_row;
	tmp = screen.line[tmp];
	tmp = file.line[tmp].line_row - screen.row_rank[screen.cur_row];
	screen.cur_row += tmp;
	if(screen.cur_row < screen.win_height)
	{
		screen.cur_col = screen.win_width;
		CursorMove();
		CheckCursor();
		getpos();
		return;
	}
	screen.cur_row -= tmp;
	CursorDown(screen.cur_row + tmp - screen.win_height - 1);
	CheckCursor();
	getpos();
}
void move_to_page_head()
{
	screen.cur_row = 1;
	screen.cur_col = screen.start_pos;
	CursorMove();
	CheckCursor();
	getpos();
	clearinbuffer();
}
void move_to_page_ass()
{
	screen.cur_row = screen.win_height/2;
	screen.cur_col = screen.start_pos;
	CursorMove();
	CheckCursor();
	getpos();
	clearinbuffer();
}
void move_to_page_tail()
{
	screen.cur_row = screen.win_height - 1;
	screen.cur_col = screen.start_pos;
	CursorMove();
	CheckCursor();
	getpos();
	clearinbuffer();
}
void jump_to_first_line()
{
	if(inbuffer.buf[inbuffer.size-1] == 'g')
	{
		file.start_line = 1;
		file.cur_line = 1;
		display(1);
		screen.cur_row = 1;
		screen.cur_col = screen.start_pos;
		CURSOR_MOVE(1,screen.start_pos);
		clearinbuffer();
		return;
	}
	addinbuffer('g');
	getpos();
}
void jump_to_end_line()
{
	if(file.total_line > screen.win_height - 2)
	{
		file.start_line = file.total_line - screen.win_height + 2;
		display(file.total_line - screen.win_height + 2);
		while(file.last_line < file.total_line)	//reach the last line.
		{
			file.start_line++;
			display(file.start_line);
		}
		file.cur_line = file.total_line;
	}
	else
	{
		display(1);
		file.start_line = 1;
	}
	screen.cur_row = screen.last_row;
	screen.cur_col = screen.start_pos;
	//CURSOR_MOVE(screen.last_row,screen.start_pos);
			 		clearinbuffer();
	getpos();
}
void move_to_index(int index)
{
	move_to_line_head();
	if(index > file.line[file.cur_line].line_end)
		exit(-1);
	while(file.cur_index != index)
		CursorRight(1);
}
int view()
{

	int cmd;
	int row,col;
	int tmp;
	static int flag = 0;
	static int save_mode = 0;
	int i;
	int modified_mode = save_mode ^ screen.view_mode;
	save_mode = screen.view_mode;
	if(flag == 0)
	{
		flag = 1;
		screen.cur_row = 1;
		screen.cur_col = 1;
		file.start_line = 1;
		screen.col_offset = 1;
		file.cur_line = 1;
		file.cur_index = 1;
		state_init();
		display(1);
		CURSOR_MOVE(1,1);
		CheckCursor();
	}
	else if (modified_mode & LINESHOW)
	{
		if(screen.view_mode & LINESHOW)
		{
			prepro();
		//	state_init();
			display(file.start_line);
			CheckCursor();
		}
		else
		{
			screen.cur_col = screen.cur_col - screen.start_pos + 1;
			screen.start_pos = 1;
		//	state_init();
			display(file.start_line);
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
        	case 'i':
					return EDIT_MODE;
			case ':':
			 		clearinbuffer();
					return CONTROL_MODE;

			case 'h':
					word_left();
					break;
			case 'j':
					line_down();
					 break;
			case 'k':
					line_up();
					break;
			case 'l':
					word_right();
					break;
			case 'g':
					jump_to_end_line();
					break;
			case 'G':
					jump_to_first_line();
					break;
			case Ctl('f'):
					page_down();
					break;

			case Ctl('b'):
					page_up();
					break;
		
			case Ctl('d'):
					half_page_down();
					break;

			case Ctl('u'):
					half_page_up();
					break;
			case 'H':
					move_to_page_head();
					break;
			case 'M':
					move_to_page_ass();
					break;
			case 'L':
					move_to_page_tail();
					break;
			case '0':
					move_to_line_head();
					break;
			case '$':
					move_to_line_tail();
					break;

			/*for debug*/
			case 's':CursorLocate(&row,&col);printf("%d %d ",row,col);fflush(stdout);	break;
			case 'w':printf("%d %d ",screen.row_offset,screen.col_offset);fflush(stdout);break;
			case 't':printf("%d ",file.total_line);fflush(stdout);break;
			case 'd':printf("%d",screen.start_pos);break;
			case 'e':printf("%d",screen.row_end[screen.cur_row]);break;
			case 'c':
					 printf("%d %d",file.cur_line,file.cur_index);
					 printf(" %d %d",screen.row_rank[screen.cur_row],file.line[file.cur_line].line_row);
					 fflush(stdout);
					 break;
			case 'p':
					 printf("%d ",file.last_line);fflush(stdout);break;
			case 'u':
					 for(i = 1 ; i <= screen.row_end[screen.cur_row - 1]; i++)
						 if(file.line[screen.line[screen.cur_row-1]].character[screen.map[screen.cur_row-1][i]] == '\t')
							 putchar('t');
						 else 
						 if(file.line[screen.cur_row - 1].character[i] == '\n')
							 putchar('n');
						 else 
						 putchar(file.line[screen.cur_row-1].character[i]);
					 break;
			/*for debug*/
            default:
					 addinbuffer(cmd);
					 break;
		}
		text_info();
	}
}
