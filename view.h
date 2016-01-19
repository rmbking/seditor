#ifndef VIEW_H
#define VIEW_H
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>	//STDIO_FILENO

#define lengthof(buf) (buf.line_size * 256 )
#define writeto(buf,word) {buf.character[buf.line_end] = word;}
extern int view();
extern void display(int line);
extern struct winsize win;

#endif
