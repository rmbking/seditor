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
	if(cur_state.character[cur_state.cur_row][cur_state.cur_col] == '\t')
	{
		cur_state.cur_col = (cur_state.cur_col + TABLEN - 1) / TABLEN * TABLEN;	
		CURSOR_MOVE(cur_state.cur_row,cur_state.cur_col);
	}

}
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

	CheckCursor();
	
}
void CursorDown(int line)
{
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
			if(cur_state.start_line + cur_state.win_height - 2 < cur_state.total_line)
			{
				cur_state.start_line++;
				display(cur_state.start_line);
				CURSOR_MOVE(cur_state.win_height-1,cur_state.cur_pos);
				cur_state.cur_col = cur_state.cur_pos;
			}
		}
	}

	CheckCursor();
}
void CursorLeft(int character)
{
	int k;
	while(cur_state.cur_col > cur_state.start_pos && character > 0)
	{
		if(cur_state.character[cur_state.cur_row][cur_state.cur_col] != '\t')
		{
			cur_state.cur_col--;
			CURSOR_LEFT();
			character--;
		}
		else
		{
			cur_state.cur_col -= TABLEN;
			k = TABLEN;
			while(k--)
				CURSOR_LEFT();
			character--;
		}
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
	cur_state.cur_pos = cur_state.cur_col;
	CheckCursor();
}
void CursorMove(int row,int col)
{

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
