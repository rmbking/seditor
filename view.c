#include "seditor.h"
void clear_screen()
{
#ifdef LINUX
//	system("echo $PS1 >tmp/ps");
//	system("export PS1=\"\"");
	system("clear");
#endif
}
void recover_screen()
{
#ifdef LINUX
//	system("export PS1=`cat tmp/ps`");
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
void process(char cmd)
{
	if(cmd == 'q'){
		recover_screen();
		mouse_show();
		exit(0);
	}
	if(cmd == 'i'){
		mouse_show();
	}
	if(cmd == 27){
		mouse_hide();
	}

}
int main(int argc,char *argv[])
{
	FILE *fp;
	char *path;
	int mode = 0;
	char word;
	char cmd;

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
	printf("end!\n");
	mouse_hide();
	fflush(stdin);
	while(cmd = getchar())
		process(cmd);

	return 0;
}
