#include "main.h"
#include "control.h"
int control()
{
	char cmd[100];
	int row,col;
	scanf("%s",cmd);
	switch(cmd[0])
	{
		/*
		case 'w':
			printf("%d %d ",cur_state.win_height,cur_state.win_width);
			break;
			*/
		case 'q':return EXIT;
		case 'n':
			cur_state.view_mode |= LINESHOW;
			return VIEW_MODE;
	}
}
