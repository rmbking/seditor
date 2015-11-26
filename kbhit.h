#ifndef KBHIT_H
#define KBHIT_H

#include <errno.h>
#include <termios.h>
#include <unistd.h>

#define SUCCESS 0
#define FAILURE -1
extern int kb_init();
extern int kb_recover();

#endif
