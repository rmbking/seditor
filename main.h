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
struct state{
	int total_line;
	int start_line;
	int cur_row;
	int cur_col;
	int line_endpos[100];	
};
extern FILE *FP;
extern struct state cur_state;
#endif
