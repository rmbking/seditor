#include "kbhit.h"
#include "main.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
	char cmd[10];
	int nReturn;
	if(kb_init() == FAILURE)	
		return FAILURE;
	if(nReturn = read(0,&cmd,3))
	{			
		if(kb_recover() == FAILURE)
			return FAILURE;
		if(nReturn == 1)
			return cmd[0];
		else if(nReturn == 3)
		{
			if(cmd[0] == '\033' && cmd[1] == '[')
			{
				switch(cmd[2])
				{
					case 'A':return 'k';
					case 'B':return 'j';
					case 'C':return 'l';
					case 'D':return 'h';
					default:return cmd[2];
				}
			}
		}
	}
}
