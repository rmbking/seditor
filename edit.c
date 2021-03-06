#include <stdlib.h>
#include <unistd.h>
#include "main.h"
#include "cursor.h"
#include "kbhit.h"
#include "control.h"
#include "view.h"

#define UP 1
#define DOWN 2

static int sameline = 0;
void prompt(int cmd)
{
	if(cmd == 1)
	{
		CURSOR_MOVE(screen.win_height,2);
		printf("----edit-mode---");
		fflush(stdout);
		CursorMove();
	}
	else
	{
		CURSOR_MOVE(screen.win_height,2);
		printf("                 ");
		fflush(stdout);
		CursorMove();
	}

}
void delete_word()
{
	int k;
	int line = file.cur_line;
	int index = file.cur_index;

	k = index-1;
	while(++k <= file.line[line].line_end)
		file.line[line].character[k-1] = file.line[line].character[k];
	file.line[line].line_end--;
	screen.row_end[screen.cur_row]--;
	CursorLeft(1);
	display(file.start_line);
	move_to_index(index - 1);
}
void insert_word(char c)
{
	int k;
	int line,index;
	if(sameline == 0)
	{
		line = file.cur_line;
		index = file.cur_index;
	}
	else
	{
		line = file.cur_line - 1;	//TODO,wrong cause the insertion may be not the line_end.
		index = file.line[line].line_end;
	}
	k = file.line[line].line_end + 1;
	if(index + 1 > lengthof(file.line[line]))		//------------------------------->to be extended for longer line	
	{
		file.line[line].character = (char *)realloc(file.line[line].character,lengthof(file.line[line]) *2);
		if(file.line[line].character == NULL)
		{
			printf("realloc error at function:%s line: %d.\n",__FUNCTION__,__LINE__);
			exit(-1);
		}
		file.line[line].line_size *= 2;
	}
	while(--k >= index)
		file.line[line].character[k+1] = file.line[line].character[k];
	file.line[line].character[index] = c;	
	file.line[line].line_end++;
	screen.row_end[screen.cur_row]++;
	if(sameline == 1)
		sameline = 0;
	if(index == file.line[line].line_end - 1 && screen.cur_col == screen.win_width)
	{
		if(screen.cur_row < screen.win_height - 1)
		{
			screen.cur_row++;
			screen.cur_col = screen.start_pos;
			CursorMove();
			sameline = 1;
		}
		else
		{
			k = file.line[file.start_line].line_row - 1;
			file.start_line++;
			display(file.start_line);
			screen.cur_row -= k;
			screen.cur_col = screen.start_pos;
			CursorMove();
			sameline = 1;
		}
		display(file.start_line);
	}
	else
	{
		/*display before cursormoving so that the the cursor will move according the modified text*/
		display(file.start_line);
		CursorRight(1);
	}
}
void divline()
{
	struct file_line *line;	//TODO,for outrange of line
	line = (struct file_line *)malloc(sizeof(struct file_line));

	/*the new line inherits the same scale as the origin*/
	*line = file.line[file.cur_line];
	line->line_end = file.line[file.cur_line].line_end - file.cur_index + 1;
	line->character = (char *)malloc(lengthof(file.line[file.cur_line]));
	if(line->character == NULL)
	{
		exit(1);
	}

	line->character[0] = '+';
	strncpy(line->character+1,file.line[file.cur_line].character+file.cur_index,line->line_end);
	file.line[file.cur_line].character[file.cur_index] = '\n';
	file.total_line++;

	insertelem(file.line,file.total_line,sizeof(struct file_line),file.cur_line+1,line);

	display(file.start_line);	//for Cursor down
	CursorDown();
	screen.cur_col = screen.start_pos;
	CursorMove();
	CheckCursor();
	getpos();
	display(file.start_line);	//for text info
}
void mergline(int dir)
{
	struct file_line *line;
	int cur_index;
	if(dir == UP)
	{
		if(file.cur_line == 1)
			return;
		line = file.line+(file.cur_line - 1);
		line->character = (char *)realloc(line->character,lengthof(*line)+ lengthof(*(line+1)));
		strncpy(line->character + line->line_end,(line+1)->character + 1,(line+1)->line_end);
		line->line_size += (line+1)->line_size;
		cur_index = line->line_end;
		line->line_end += (line+1)->line_end - 1;
		deletelem(file.line,file.total_line,sizeof(struct file_line),file.cur_line);
		file.total_line--;
		display(file.start_line);
		line_up();
		move_to_index(cur_index);
	}
	if(dir == DOWN)
	{
		if(file.cur_line == file.total_line)
			return;
		line = file.line + file.cur_line;
		line->character = (char *)realloc(line->character,lengthof(*line)+ lengthof(*(line+1)));
		strncpy(line->character + line->line_end,(line+1)->character + 1,(line+1)->line_end);
		line->line_size += (line+1)->line_size;
		line->line_end += (line+1)->line_end - 1;
		deletelem(file.line,file.total_line,sizeof(struct file_line),file.cur_line+1);
		file.total_line--;
		display(file.start_line);
	}
}

int edit()
{
	int word;
	prompt(1);
	/*move the cursor to the most left position of the TAb when editing*/
	if('\t' == CUR_WORD)
		while(screen.cur_col != screen.start_pos && screen.map[screen.cur_row][screen.cur_col] == screen.map[screen.cur_row][screen.cur_col - 1])
		{
			screen.cur_col--;
			CursorMove();
		}
	while((word = kb_input()) != '\033')	
	{
		switch(word)
		{
			case BACKSPACE:
				if(file.cur_index != 1)
					delete_word();
				else
					mergline(UP);
				break;
			case DEL:
				if(file.cur_index != file.line[file.cur_line].line_end)
				{
					CursorRight(1);
					delete_word();
				}
				else
					mergline(DOWN);
				break;
			case '\n':
				divline();
				break;
			default:
				insert_word(word);
				break;
		}
		prompt(1);
		if('\t' == CUR_WORD)
			while(screen.cur_col != screen.start_pos && screen.map[screen.cur_row][screen.cur_col] == screen.map[screen.cur_row][screen.cur_col - 1])
			{
				screen.cur_col--;
				CursorMove();
			}
	}
	prompt(0);
	screen.view_mode &= ~SYNC;
	return VIEW_MODE;
}
