#include <stdarg.h>
#include "kbhit.h"
#include "view.h"
#include "main.h"
#include "control.h"
#include "cursor.h"
void control_info_print(char *fmt,...)
{
	va_list args;
	CURSOR_MOVE(cur_state.win_height,2);
	va_start(args,fmt);
	vprintf(fmt,args);
	va_end(args);
	CURSOR_MOVE(cur_state.cur_row,cur_state.cur_col);
}
void reset_endrow()
{
	int i;
	CURSOR_MOVE(cur_state.win_height,1);
	for(i = 1; i < cur_state.win_width; i++)	//not print at the last position in case that the cursor change line.
		printf(" ");
	text_info();
}
void input(char *cmd)
{
	char tmp,*pos;
	reset_endrow();
	CURSOR_MOVE(cur_state.win_height,1);	
	printf(":");
	fflush(stdout);
	tmp = kb_input();
	pos = cmd;
	while(tmp != '\n' && (pos - cmd) < cur_state.win_width - 25 )
	{
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
int exe_quit(char *cmd)
{
	if(cmd[1] == '\0')
		return EXIT;
	control_info_print("Unknown command:%s",cmd);
	return VIEW_MODE;
}
int exe_list_number(char *cmd,int set)
{

	if(set)
	{
		if(cmd[1] == '\0')
		{
			cur_state.view_mode |= LINESHOW;
			return VIEW_MODE;
		}
		control_info_print("Unknown command:%s",cmd);
		return VIEW_MODE;
	}
	else
	{
		if(cmd[1] == '\0')	
		{
			cur_state.view_mode &= ~LINESHOW;
			return VIEW_MODE;
		}
		control_info_print("Unknown command:%s",cmd);
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
		/*
		case 'w':
			printf("%d %d ",cur_state.win_height,cur_state.win_width);
			break;
			*/
		case 'q':
			return exe_quit(cmd);
		case 'n':
			return exe_list_number(cmd,1);
		case '!':
			switch(cmd[1])
			{
				case 'n':
					return exe_list_number(cmd+1,0);	
				default:
					control_info_print("Unknown command:%s",cmd);
					return VIEW_MODE;
			}
		default:
			control_info_print("Unknown command:%s",cmd);
			return VIEW_MODE;
	}
}
