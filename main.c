#include <stdio.h>
#include "main.h"
#include "kbhit.h"
#include "cursor.h"
#include "control.h"

FILE *FP,*OFP;
struct state cur_state;
struct buffer inbuffer;
void addinbuffer(char c)
{
	int k;
	if(inbuffer.size < BUFLEN)
		inbuffer.buf[inbuffer.size++] = c;
	else
	{
		for(k = 0; k < BUFLEN - 1; k++)
			inbuffer.buf[k] = inbuffer.buf[k+1];
		inbuffer.buf[BUFLEN-1] = c;
	}
}
void clearinbuffer()
{
	inbuffer.size = 0;
}
void clear_screen()
{
#ifdef LINUX
	printf("\033[2J\033[1;1H");
#endif
}
void init()
{
	memset(&cur_state,0,sizeof(cur_state));	
	cur_state.start_line = 1;
	cur_state.cur_pos = 1;
	cur_state.start_pos = 1;
}
void text_info()
{
	int cur_line;
	cur_line = cur_state.start_line + cur_state.cur_row - 1;
	CURSOR_MOVE(cur_state.win_height,cur_state.win_width-20);
	printf("                    ");
	CURSOR_MOVE(cur_state.win_height,cur_state.win_width-20);
	printf("line:%d",cur_line);
	CURSOR_MOVE(cur_state.win_height,cur_state.win_width-5);
	printf("%d%%",cur_line * 100 / cur_state.total_line);

	CURSOR_MOVE(cur_state.cur_row,cur_state.cur_col);
}
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
	init();
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
void fcopy(FILE *tfp,FILE *sfp)
{
	char word;
	while((word = fgetc(sfp)) != EOF)
		fputc(word,tfp);
}
int frm(char *path)
{
	if(remove(path))
		return	-1;
   return 0;	
}
int main(int argc,char *argv[])
{
	char *path;
	int mode = 0;

	path = argv[1];
	check(&mode,path);

	if(mode == WRITE || mode == NONE){
		printf("Pemission denied to open the file.\n");
		exit(1);
	}
	if(mode == READ)
	{
		OFP = fopen(path,"r");
		path = strcat(path,"_tmp");
		FP = fopen(path,"w+");
		fcopy(FP,OFP);
	}

	if(mode == BOTH || mode ==NEW)
	{
		OFP = fopen(path,"r+");
		path = strcat(path,"_tmp");
		FP = fopen(path,"w+");
		fcopy(FP,OFP);
	}
	
	clear_screen();	
	process();
	fclose(OFP);
	fclose(FP);
//	frm(path);
	clear_screen();
	return 0;
}
