#include "main.h"
#include "cursor.h"
#include "kbhit.h"
#include "control.h"
#include "view.h"
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
		line = file.cur_line - 1;
		index = file.line[line].line_end;
	}
	k = file.line[line].line_end + 1;
	if(index + 1 <= lengthof(file.line[line]))		//------------------------------->to be extended for longer line	
	{
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
}
int edit()
{
	int word;
	prompt(1);
	while((word = kb_input()) != '\033')	
	{
		switch(word)
		{
			case 127:
				delete_word();
				break;
			case DEL:
				if(file.cur_index != file.line[file.cur_line].line_end)
				{
					CursorRight(1);
					delete_word();
				}
				break;
			default:
				insert_word(word);
				break;
		}
		prompt(1);
	}
	prompt(0);
	return VIEW_MODE;
}
