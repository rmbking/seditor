#ifndef VIEW_H
#define VIEW_H
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>	//STDIO_FILENO

#define lengthof(buf) ((buf).line_size * 256 )
#define writeto(buf,word) {(buf).character[(buf).line_end] = (word);}
extern int view();
extern void display(int line);
extern struct winsize win;

extern void line_down();
extern void line_up();

extern void word_left();
extern void word_right();

extern void page_down();
extern void page_up();

extern void half_page_down();
extern void half_page_up();

extern void move_to_line_head();
extern void move_to_line_tail();

extern void move_to_page_head();
extern void move_to_page_ass();
extern void move_to_page_tail();

extern void jump_to_first_line();
extern void jump_to_end_line();

extern void move_to_index(int index);
extern void ftob();
#endif
