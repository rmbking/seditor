#include <stdio.h>
#include <termios.h>
#include <string.h>
#include "main.h"
#include "cursor.h"
#include "view.h"
void CheckCursor()
{

	if(cur_state.cur_row > cur_state.last_row)
	{
		CURSOR_MOVE(cur_state.last_row,cur_state.start_pos);
		cur_state.cur_row = cur_state.last_row;
	}

	if(cur_state.line_endpos[cur_state.cur_row] < cur_state.cur_col)	//not move the cursor to the position where no character exits.
	{
		CURSOR_MOVE(cur_state.cur_row,cur_state.line_endpos[cur_state.cur_row]);
		cur_state.cur_col = cur_state.line_endpos[cur_state.cur_row];
	}

	if(cur_state.view_mode & LINESHOW && cur_state.cur_col < cur_state.start_pos)
	{
		CURSOR_MOVE(cur_state.cur_row,cur_state.start_pos);
		cur_state.cur_col = cur_state.start_pos;
	}
	if(file.line[file.cur_line].character[file.cur_index] == '\t')
	{
		cur_state.cur_col = (cur_state.cur_col + TABLEN - 1) / TABLEN * TABLEN;	
		CURSOR_MOVE(cur_state.cur_row,cur_state.cur_col);
	}

}
static int row_of_line = 1;
void CursorUp(int line)
{
	while(line--)
	{
		if(cur_state.cur_row > 1)	
		{
			cur_state.cur_row--;
			CURSOR_MOVE(cur_state.cur_row,cur_state.cur_pos);
			cur_state.cur_col = cur_state.cur_pos;
		}
		else
		{
			if(cur_state.start_line > 1)
			{
				cur_state.start_line--;
				display(cur_state.start_line);
				CURSOR_MOVE(1,cur_state.cur_pos);
				cur_state.cur_col = cur_state.cur_pos;
			}
		}
	}
	file.cur_line--;
	CheckCursor();
}
void CursorDown(int line)
{
	int back_row;
	while(line--)
	{
		if(cur_state.cur_row < cur_state.win_height - 1)
		{
			cur_state.cur_row++;
			CURSOR_MOVE(cur_state.cur_row,cur_state.cur_pos);
			cur_state.cur_col = cur_state.cur_pos;
		}
		else
		{
			if(cur_state.start_line  <= file.total_line)
			{
				back_row = file.line[cur_state.start_line].line_row;	//start_line.row > 1 ,so move back the cursor
				cur_state.start_line++;
				display(cur_state.start_line);
				cur_state.cur_row = cur_state.win_height-back_row;
				cur_state.cur_col = cur_state.cur_pos;
				CursorMove();
			}
		}
	}
	file.cur_line++;
	CheckCursor();
}
void CursorLeft(int character)
{
	int k;
	while(cur_state.cur_col > cur_state.start_pos && character > 0)
	{
		
		if(file.line[file.cur_line].character[file.cur_index] != '\t')
		{
			cur_state.cur_col--;
			CURSOR_LEFT();
			character--;
		}
		else
		{
			if(cur_state.cur_col > TABLEN)
				cur_state.cur_col -= TABLEN;
			k = TABLEN;
			while(k--)
				CURSOR_LEFT();
			character--;
		}

		if(file.cur_index > 1)
			file.cur_index --;
	}
	cur_state.cur_pos = cur_state.cur_col;	//used for cursor up-moving and down-moving
	CheckCursor();
}
void CursorRight(int character)
{
	while(cur_state.cur_col < cur_state.line_endpos[cur_state.cur_row] && character > 0)
	{
		cur_state.cur_col++;
		CURSOR_RIGHT();
		character--;
	}
	if(file.cur_index < file.line[file.cur_line].line_end)
		file.cur_index ++;
	cur_state.cur_pos = cur_state.cur_col;
	CheckCursor();
}
void CursorMove()
{
	CURSOR_MOVE(cur_state.cur_row,cur_state.cur_col);
}
void CursorLocate(int *row,int *col)
{
	struct termios saveSettings,newSettings;
	char out[10];
	char *tmp;
	int flag = 0;
	if(tcgetattr(1,&saveSettings) != 0)
	{
		perror("tcgetattr");
		return;
	}

	newSettings = saveSettings;
	newSettings.c_lflag &= ~ICANON;	//turn on the canonical mode,so other settings can work 
	newSettings.c_lflag &= ~ECHO;		//no echos
	newSettings.c_cc[VMIN] = 1;		//at least get one character
	newSettings.c_cc[VTIME] = 0;	//no time limit

	if(tcsetattr(0,TCSANOW,&newSettings) != 0)	//be in use immediately
	{
		perror("tcsetattar");
		return;
	}

	printf("\033[6n");	//query the cursor position
	fflush(stdout);
	read(1,out,10);

	tmp = strtok(out,"[R;");	//the answer's format is like *[<row>;<col>R
	tmp = strtok(NULL,"[R;");
	*row = atoi(tmp);
	tmp = strtok(NULL,"[R;");
	*col = atoi(tmp);
	
	if(tcsetattr(0,TCSANOW,&saveSettings))		//recover the settings
	{
		perror("tcsetattr");
		return;
	}
}
