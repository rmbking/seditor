#ifndef MAIN_H
#define MAIN_H

#define LINUX
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#define NONE 0
#define READ 1
#define WRITE 2
#define BOTH 3
#define NEW 4

#define EXIT 0
#define VIEW_MODE 1
#define EDIT_MODE 2
#define CONTROL_MODE 4

#define SUCCESS 0
#define FAILURE -1

#define TABLEN 4
#define BUFLEN 10

#define LINESHOW  01
#define MODIFIED  02

#define MAX_SCREEN_HEIGHT 100
#define MAX_SCREEN_WIDTH 200
#define Ctl(x) (x - 'a' + 1)
struct file_line{
	int line_end;
	int line_row;
	int line_size;	//size / 256
	char *character;	
};
struct state{
	int total_line;
	int start_line;
	int win_height;
	int view_mode;
	int win_width;
	int cur_pos;	//the cursor's position when it moves up and down
	int start_pos;
	int cur_row;
	int last_row;
	int cur_col;
	int line_endpos[MAX_SCREEN_HEIGHT];	
	struct flie_line *line;
};
struct buffer{
	char buf[10];
	int size;
};
extern FILE *FP,*OFP;
extern struct state cur_state;
extern struct buffer inbuffer;

extern void addinbuffer(char c);
extern void clearinbuffer();

extern void text_info();
extern void fcopy(FILE *tfp,FILE *sfp);
#endif
