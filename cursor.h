#ifndef CURSOR_H
#define CURSOR_H

#define CURSOR_HIDE() printf("\033[?25l");fflush(stdout); 
#define CURSOR_SHOW() printf("\033[?25h");fflush(stdout);

#define CURSOR_RECORD() printf("\033[s");ffush(stdout);
#define CURSOR_BACK() printf("\033[u");fflush(stdout);

#define CURSOR_SELECTED() printf("\033[7m");fflush(stdout);
#define CURSOR_BLINK() printf("\033[5m");fflush(stdout);
#define CURSOR_UNBLINK() printf("\033[37m");fflush(stdout);
#define CURSOR_UNDERLINE() printf("\033[4m");fflush(stdout);

#define CURSOR_UP() {printf("\033[1A");fflush(stdout);}
#define CURSOR_DOWN() {printf("\033[1B");fflush(stdout);}
#define CURSOR_LEFT() {printf("\033[1D");fflush(stdout);}
#define CURSOR_RIGHT() {printf("\033[1C");fflush(stdout);}

#define CURSOR_MOVE(X,Y) {printf("\033[%d;%dH",X,Y);fflush(stdout);}

extern void CursorUp(int line);
extern void cursorDown(int line);
extern void CursorLeft(int character);
extern void CursorRight(int character);

extern void CursorMove();
extern void CursorLocate(int *row,int *col);
#endif
