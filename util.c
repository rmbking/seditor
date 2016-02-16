#include <stdio.h>
#include <string.h>
#include "util.h"

int insertelem(void *base,int nmem,int size,int pos,void *elem)
{
	char *p = (char *)base;
	p += nmem*size;
	while(pos < nmem)
	{
		memcpy(p,p-size,size);	
		pos++;
		p -= size;
	}
	memcpy(p,elem,size);	
}
int deletelem(void *base,int nmem,int size,int pos)
{
	char *p = (char *)base;
	p += pos*size;
	while(pos <= nmem)
	{
		memcpy(p,p+size,size);
		pos++;
		p += size;
	}
	return 1;
}
