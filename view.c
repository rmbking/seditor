#include <stdio.h>
#include "view.h"
#include "main.h"
#include "cursor.h"
int view()
{
	char cmd;
    while(cmd = kb_input())
    {
    	if(cmd == FAILURE)
        {
        	printf("Something is wrong the keyboard.\n");
            exit(2);
        }
        switch(cmd)
        {
        	case 'i':return EDIT_MODE;

			case 'h':CURSOR_LEFT();break;
			case 'j':CURSOR_DOWN();break;
			case 'k':CURSOR_UP();break;
			case 'l':CURSOR_RIGHT();break;
            default:printf("%c is not a valid operation.\n",cmd);
		}
	}
}

