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
void input(char *cmd)
{
	char tmp,*pos;
	CURSOR_MOVE(cur_state.win_height,1);	
	printf(":");
	fflush(stdout);
	tmp = kb_input();
	pos = cmd;
	while(tmp != '\n' && (pos - cmd) < cur_state.win_width - 25 )
	{
		*pos++ = tmp;
		printf("%c",tmp);
		fflush(stdout);
		tmp = kb_input();
	}
	*pos = '\0';

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
			if(cmd[1] == '\0')
				return EXIT;
			control_info_print("Unknown command:%s",cmd);
			return VIEW_MODE;
		case 'n':
			cur_state.view_mode |= LINESHOW;
			return VIEW_MODE;
		default:
			control_info_print("Unknown command:%s",cmd);
			return VIEW_MODE;
	}
}
