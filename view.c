#include <stdio.h>
#include "view.h"
#include "main.h"
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
        	case 'i':kb_recover();return EDIT_MODE;
			case 'q':kb_recover();return EXIT;
            default:printf("%c is not a valid operation.\n",cmd);
		}
	}
}

