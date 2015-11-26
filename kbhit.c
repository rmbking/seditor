#include "kbhit.h"
#include <stdio.h>
struct termios save,new;
int  kb_init()
{
	if( tcgetattr(0,&save) != 0 )	
	{
		perror("tcgetattr");
		return FAILURE;
	}
	new = save;
	new.c_lflag &= ~ICANON;
	new.c_lflag &= ~ECHO;
	new.c_cc[VMIN] = 1;
	new.c_cc[VTIME] = 0;
	if( tcsetattr(0,TCSANOW,&new) != 0 )
	{
		perror("tcsetattr");
		return FAILURE;
	}
	return SUCCESS;

}
int kb_recover()
{
	if( tcsetattr(0,TCSANOW,&save) != 0)
	{
		perror("tcsetattr");
		return FAILURE;
	}
	return SUCCESS;
}
int kb_input()
//int main()
{
	char cmd = '0';
	if(kb_init() == FAILURE)	
		return FAILURE;
	if(read(0,&cmd,1))
	{			
		if(kb_recover() == FAILURE)
			return FAILURE;
		return cmd;
	}
}
