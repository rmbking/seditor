#include <stdarg.h>
#include "kbhit.h"
#include "view.h"
#include "main.h"
#include "control.h"
#include "cursor.h"
void control_info_print(char *fmt,...)
{
	va_list args;
	CURSOR_MOVE(screen.win_height,2);
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	CURSOR_MOVE(screen.cur_row,screen.cur_col);
}
void reset_endrow()
{
	int i;
	CURSOR_MOVE(screen.win_height,1);
	for(i = 1; i < screen.win_width; i++)	//not print at the last position in case that the cursor change line.
		printf(" ");
	text_info();
}
void input(char *cmd)
{
	char tmp,*pos;
	reset_endrow();
	CURSOR_MOVE(screen.win_height,1);	
	printf(":");
	fflush(stdout);
	tmp = kb_input();
	pos = cmd;
	while(tmp != '\n' && (pos - cmd) < screen.win_width - 25 )
	{
		if(tmp == '\033')
		{
			cmd[0] = '\033';
			break;
		}
		if(tmp == 127)	//backspace
		{
			*(--pos) = '\0';
			CURSOR_LEFT();	
			printf(" ");
			CURSOR_LEFT();	
			tmp = kb_input();
			continue;
		}
		*pos++ = tmp;
		printf("%c",tmp);
		fflush(stdout);
		tmp = kb_input();
	}
	*pos = '\0';

}
int exe_quit(char *cmd,int set)
{
	if(set)
	{
		if(cmd[1] == '\0')
			if(screen.view_mode & SYNC)
				return EXIT;
			else
			{
				control_info_print("file should be saved first or you can use '!q' to forcely quit");
				return VIEW_MODE;
			}
		else
		{
			control_info_print("Unknown command:%s",cmd);
			return VIEW_MODE;
		}
	}
	else
	{
		if(cmd[1] == '\0')
			return EXIT;
		control_info_print("Unknown command:%s",cmd-1);
		return VIEW_MODE;
	}
}
int exe_list_number(char *cmd,int set)
{

	if(set)
	{
		if(cmd[1] == '\0')
		{
			screen.view_mode |= LINESHOW;
			return VIEW_MODE;
		}
		control_info_print("Unknown command:%s",cmd-1);
		return VIEW_MODE;
	}
	else
	{
		if(cmd[1] == '\0')	
		{
			screen.view_mode &= ~LINESHOW;
			return VIEW_MODE;
		}
		control_info_print("Unknown command:%s",cmd-1);
		return VIEW_MODE;
	}
}
int exe_save_or_cancel(const char *cmd,int set)
{
	if(set)
	{
		if(cmd[1] == '\0')
		{
			btof(FP);
			screen.view_mode |= SYNC;
			return VIEW_MODE;
		}
		control_info_print("Unknown command:%s",cmd);
		return VIEW_MODE;
	}
	else
	{
		if(cmd[1] == '\0')	
		{	
			ftob();				//TODO,not ready to work
			return VIEW_MODE;
		}
		control_info_print("Unknown command:%s",cmd-1);
		return VIEW_MODE;
	}
	
}
int control()
{
	char cmd[100];
	int row,col;
	input(cmd);
	switch(cmd[0])
	{
		case 'q':
			return exe_quit(cmd,1);
		case 'n':
			return exe_list_number(cmd,1);
		case 'w':
			return exe_save_or_cancel(cmd,1);
		case '!':
			switch(cmd[1])
			{
				case 'q':
					return exe_quit(cmd+1,0);
				case 'n':
					return exe_list_number(cmd+1,0);	
				case 'w':
					return exe_save_or_cancel(cmd+1,0);
				default:
					control_info_print("Unknown command:%s",cmd);
					return VIEW_MODE;
			}
		case '\033':
			return VIEW_MODE;
		default:
			control_info_print("Unknown command:%s",cmd);
			return VIEW_MODE;
	}
}
