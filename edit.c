#include "main.h"
#include "cursor.h"
#include "kbhit.h"
#include "control.h"
#include "view.h"
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
}
void insert_word(char c)
{
	int k;
	int line = file.cur_line;
	int index = file.cur_index;
	k = file.line[line].line_end + 1;
	if(index + 1 <= lengthof(file.line[line]))		
	{
		while(--k >= index)
			file.line[line].character[k+1] = file.line[line].character[k];
		file.line[line].character[index] = c;	
		file.line[line].line_end++;
		screen.row_end[screen.cur_row]++;
		CursorRight(1);
	}
		
}
int edit()
{
	char word;
	prompt(1);
	while((word = kb_input()) != '\033')	
	{
		switch(word)
		{
			case 127:
				delete_word();
				display(file.start_line);
				prompt(1);
				break;
			default:
				insert_word(word);
				display(file.start_line);
				prompt(1);
				break;
		}
	}
	prompt(0);
	return VIEW_MODE;
}
