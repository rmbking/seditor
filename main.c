#include <stdio.h>
#include "main.h"
#include "kbhit.h"
void clear_screen()
{
#ifdef LINUX
	system("clear");
#endif
}
void mouse_hide()
{
#ifdef LINUX
	system("echo  \"\\033[?25l\"");
#endif
}
void mouse_show()
{
#ifdef LINUX
	system("echo  \"\\033[?25h\"");
#endif
}
int edit()
{}
int control()
{}
void check(int *mode,char * path)
{
	struct stat file_info;
	struct stat *file_info_p = &file_info;

	if(stat(path,file_info_p) == -1){
	
		switch(errno){
		
			case EACCES:
				printf("Permission denied to search one one the directories.\n");
				exit(1);
			case ENAMETOOLONG:
				printf("Path is too long.\n");
				exit(1);
			case ENOTDIR:
				printf("Some directories do not exit.");
				exit(1);
			case ENOENT:
			   	*mode = NEW;	
				return;
		}
	}
	
	if(!S_ISREG(file_info_p->st_mode)){

		printf("%s is not a regular file.\n",path);
		exit(1);
	}

	if(file_info_p->st_mode & S_IRUSR)
		*mode |= READ;
	if(file_info_p->st_mode & S_IWUSR)
		*mode |= WRITE;


}
void process()
{
	int mode = VIEW_MODE;
	while(mode != EXIT)
	{
		switch(mode)
		{
			case VIEW_MODE:mode = view();break;
			case EDIT_MODE:mode = edit();break;
			case CONTROL_MODE:mode = control();break;
			case EXIT:break;
			default: printf("[view.c][process]:error,invalid mode.");
		}
	}
}
int main(int argc,char *argv[])
{
	FILE *fp;
	char *path;
	int mode = 0;
	char word;

	path = argv[1];
	check(&mode,path);

	if(mode == WRITE || mode == NONE){
		printf("Pemission denied to open the file.\n");
		exit(1);
	}
	if(mode == READ)
		fp = fopen(path,"r");
	if(mode == BOTH || mode ==NEW)
		fp = fopen(path,"r+");
	
	clear_screen();	
	while((word = fgetc(fp)) != EOF)
		putchar(word);
	mouse_hide();
	process();
	mouse_show();
	return 0;
}
