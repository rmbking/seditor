#ifndef KBHIT_H
#define KBHIT_H

#include <errno.h>
#include <termios.h>
#include <unistd.h>

extern int kb_init();
extern int kb_recover();
extern int kb_input();

#endif
