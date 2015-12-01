#ifndef VIEW_H
#define VIEW_H
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>	//STDIO_FILENO
extern int view();
extern int cur_line;
extern int cur_pos;
extern int total_line;
extern struct winsize win;

#endif
