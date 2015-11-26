#ifndef SEDITOR_H
#define SEDITOR_H

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

#endif
