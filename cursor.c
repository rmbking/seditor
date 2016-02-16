#include <stdio.h>
#include <termios.h>
#include <string.h>
#include "main.h"
#include "cursor.h"
#include "view.h"
void offset()
{
	screen.col_offset = screen.cur_col;	//used for cursor up-moving and down-moving
	screen.row_offset = screen.row_rank[screen.cur_row];
}
void CursorMove()
{
	CURSOR_MOVE(screen.cur_row,screen.cur_col);
}
void CheckCursor()
{
	getpos();

	if(screen.cur_row > screen.last_row)
	{
		screen.cur_row = screen.last_row;
	}

	if(screen.row_end[screen.cur_row] < screen.cur_col)	//not move the cursor to the position where no character exits.
	{
		screen.cur_col = screen.row_end[screen.cur_row];
	}

	if(screen.cur_col < screen.start_pos)
	{
		screen.cur_col = screen.start_pos;
	}
	if(file.line[file.cur_line].character[file.cur_index] == '\t')
	{
		screen.cur_col = (screen.cur_col + TABLEN - 1) / TABLEN * TABLEN;	
	}

	CursorMove();

}
void CursorUp(int line)
{
	int rows;
	if(file.cur_line > 1)
	{
		if(file.cur_line > file.start_line)
		{
			if(file.line[file.cur_line - 1].line_row < screen.row_offset)
			{
				rows = screen.row_rank[screen.cur_row ];
				screen.cur_col = screen.win_width;
			}
			else
			{
				rows = screen.row_rank[screen.cur_row] + file.line[file.cur_line-1].line_row - screen.row_offset;
				screen.cur_col = screen.col_offset;
			}
			while(rows--)

				screen.cur_row--;
		}	//not preread the former line,may cause bug.
		else
		{
			file.start_line--;
			display(file.start_line);
			if(file.line[file.start_line].line_row < screen.row_offset)
			{
				screen.cur_row = file.line[file.start_line].line_row;
				screen.cur_col = screen.win_width;
			}	
			else
			{
				screen.cur_row = screen.row_offset;
				screen.cur_col = screen.col_offset;	
			}
		}
		CursorMove();
		getpos();
		CheckCursor();
	}
}
void CursorDown(int line)
{
	int rows;
	int lines;
	if(file.cur_line < file.total_line)
	{
		if(screen.row_offset > file.line[file.cur_line+1].line_row)
		{
			screen.cur_col = screen.win_width;
			lines = file.line[file.cur_line].line_row - screen.row_rank[screen.cur_row] + file.line[file.cur_line + 1].line_row;
		}
		else
		{
			screen.cur_col = screen.col_offset;
			lines = file.line[file.cur_line].line_row - screen.row_rank[screen.cur_row] + screen.row_offset;
		}
		while(lines--)
		{
			if(screen.cur_row < screen.win_height - 1)
			{
				screen.cur_row++;
			}
			else
			{
				if(file.start_line  <= file.total_line)
				{
					rows = file.line[file.start_line].line_row;	//start_line.row > 1 ,so move back the cursor
					file.start_line++;
					display(file.start_line);
					screen.cur_row = screen.win_height-rows;
				}
			}
		}
		CursorMove();
		getpos();
		CheckCursor();
	 }
}
void CursorLeft(int character)
{
	int k;
	while(file.cur_index > 1 && character > 0)
	{
		if(screen.cur_col == screen.start_pos || screen.cur_col < (screen.start_pos + TABLEN - 1) &&  file.line[file.cur_line].character[file.cur_index]== '\t')	
		{
			screen.cur_col = screen.win_width;//D:to be extended if direction keys are used since the first row not the fisrt.	
			screen.cur_row--;
		}
		else
		{
			if(file.line[file.cur_line].character[file.cur_index] != '\t')
			{
				CURSOR_LEFT();
				screen.cur_col--;
				}
			else
			{
				for(k = 1; k < TABLEN; k++)
				{
					if(screen.map[screen.cur_row][screen.cur_col-k+1] == screen.map[screen.cur_row][screen.cur_col-k])
					{
						CURSOR_LEFT();
					}
					else
						break;
				}
				screen.cur_col -= k;
			}
		}

		character--;

		if(file.cur_index > 1)
			file.cur_index --;
	}
	offset();
	CheckCursor();
}
void CursorRight(int character)
{
	int rows;
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
			//if the last character is right in the last column,the newline won't occupy a new line.
			if(screen.cur_row < screen.win_height - 1 && file.cur_index < file.line[file.cur_line].line_end - 1)
			{
				screen.cur_col = screen.start_pos;
				screen.cur_row ++;
				CursorMove();
			}
			else if(screen.cur_row == screen.win_height - 1)
			{
				rows = file.line[file.start_line].line_row - 1;
				file.start_line++;//not checked,in fact it will cause bugs but I don't want to deal now.
				display(file.start_line);
				screen.cur_row -= rows;
				screen.cur_col = screen.start_pos;
				CursorMove();
			}
		}
		character--;
	}

	if(file.cur_index < file.line[file.cur_line].line_end)
		file.cur_index ++;
	offset();
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
