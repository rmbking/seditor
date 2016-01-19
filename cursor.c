#include <stdio.h>
#include <termios.h>
#include <string.h>
#include "main.h"
#include "cursor.h"
#include "view.h"
void CheckCursor()
{

	if(screen.cur_row > screen.last_row)
	{
		CURSOR_MOVE(screen.last_row,screen.start_pos);
		screen.cur_row = screen.last_row;
	}

	if(screen.row_end[screen.cur_row] < screen.cur_col)	//not move the cursor to the position where no character exits.
	{
		CURSOR_MOVE(screen.cur_row,screen.row_end[screen.cur_row]);
		screen.cur_col = screen.row_end[screen.cur_row];
	}

	if(screen.view_mode & LINESHOW && screen.cur_col < screen.start_pos)
	{
		CURSOR_MOVE(screen.cur_row,screen.start_pos);
		screen.cur_col = screen.start_pos;
	}
	if(file.line[file.cur_line].character[file.cur_index] == '\t')
	{
		screen.cur_col = (screen.cur_col + TABLEN - 1) / TABLEN * TABLEN;	
		CURSOR_MOVE(screen.cur_row,screen.cur_col);
	}

}
static int row_of_line = 1;
void CursorMove()
{
	CURSOR_MOVE(screen.cur_row,screen.cur_col);
}
void CursorUp(int line)
{
	while(line--)
	{
		if(screen.cur_row > 1)	
		{
			screen.cur_row--;
			CURSOR_MOVE(screen.cur_row,screen.col_offset);
			screen.cur_col = screen.col_offset;
		}
		else
		{
			if(file.start_line > 1)
			{
				file.start_line--;
				display(file.start_line);
				CURSOR_MOVE(1,screen.col_offset);
				screen.cur_col = screen.col_offset;
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
		if(screen.cur_row < screen.win_height - 1)
		{
			screen.cur_row++;
			CURSOR_MOVE(screen.cur_row,screen.col_offset);
			screen.cur_col = screen.col_offset;
		}
		else
		{
			if(file.start_line  <= file.total_line)
			{
				back_row = file.line[file.start_line].line_row;	//start_line.row > 1 ,so move back the cursor
				file.start_line++;
				display(file.start_line);
				screen.cur_row = screen.win_height-back_row;
				screen.cur_col = screen.col_offset;
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
	while(screen.cur_col > screen.start_pos && character > 0)
	{
		
		if(file.line[file.cur_line].character[file.cur_index] != '\t')
		{
			screen.cur_col--;
			CURSOR_LEFT();
			character--;
		}
		else
		{
			for(k = 0; k < TABLEN-1; k++)
				if(screen.map[screen.cur_row][screen.cur_col-k] == screen.map[screen.cur_row][screen.cur_col-k-1])
				{
					CURSOR_LEFT();
				}
				else
					break;
			CURSOR_LEFT();
			screen.cur_col -= k+1;
			character--;
		}

		if(file.cur_index > 1)
			file.cur_index --;
	}
	screen.col_offset = screen.cur_col;	//used for cursor up-moving and down-moving
	CheckCursor();
}
void CursorRight(int character)
{
	/*may be used as the extension of direction keys
	while(screen.cur_col < screen.row_end[screen.cur_row] && character > 0)
	{
		screen.cur_col++;
		CURSOR_RIGHT();
		character--;
	}
	*/
	while(character > 0 && file.cur_index < file.line[file.cur_line].line_end)
	{
		if(screen.cur_col < screen.win_width)
		{
			screen.cur_col++;
			CURSOR_RIGHT();
		}
		else
		{
			screen.cur_col = screen.start_pos;
			screen.cur_row ++;
			CursorMove();
		}
		character--;
	}

	if(file.cur_index < file.line[file.cur_line].line_end)
		file.cur_index ++;
	screen.col_offset ++;
	CheckCursor();
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
