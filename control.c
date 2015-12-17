#include "main.h"
#include "control.h"
int control()
{
	char cmd[100];
	scanf("%s",cmd);
	if(strcmp(cmd,"q"))
		return EXIT;
}
