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
struct file{
	int open_mode;
	int start_line;
	int total_line;
	int last_line;
	int cur_line;
	int cur_index;
	struct file_line *line;
};
struct screen{
	int win_height;
	int win_width;
	int view_mode;
	int cur_row;
	int cur_col;
	int start_pos;
	int cur_pos;
	int last_row;
	int row_end[MAX_SCREEN_HEIGHT];
	int row_rank[MAX_SCREEN_HEIGHT];
	int line[MAX_SCREEN_HEIGHT];
	int map[MAX_SCREEN_HEIGHT][MAX_SCREEN_WIDTH];
};
/*
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
	int last_line;
	int cur_col;
	int cur_line;
	int cur_index;
	int line_endpos[MAX_SCREEN_HEIGHT];	
	struct file_line *line;
//	int map[MAX_SCREEN_HEIGHT][MAX_SCREEN_WIDTH];	//screen point to charcater index.
};
*/
struct buffer{
	char buf[10];
	int size;
};
extern FILE *FP,*OFP;
extern struct state cur_state;
extern struct file file;
extern struct screen screen;
extern struct buffer inbuffer;

extern void addinbuffer(char c);
extern void clearinbuffer();

extern void text_info();
extern void fcopy(FILE *tfp,FILE *sfp);
extern int frm(char *fullname);
extern void btof(FILE *fp);
#endif
