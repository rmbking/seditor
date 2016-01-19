#include "main.h"
#include "kbhit.h"
#include "control.h"
#include "view.h"
void insert_word(char c)
{
	int k;
	int line = file.cur_line;
	int index = file.cur_index;
	k = file.line[line].line_end + 1;
	if(index + 1 <= lengthof(file.line[line]))		
	{
		while(--k >= index)
			file.line[line].character[k+1] = file.line[line].character[k];
		file.line[line].character[index] = c;	
		CursorRight(1);
		file.line[file.cur_line].line_end++;
	}
		
}
int edit()
{
	char word;
	while((word = kb_input()) != 'q')	
	{
		insert_word(word);
		display(file.start_line);
	}
	return EXIT;
}
