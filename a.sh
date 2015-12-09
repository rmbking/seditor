#!/bin/bash
str=`tail -1 eof | hexdump |  awk '{print $2}'`
if [ "$str" = ffff ]
then
	sed -i '$d' eof
fi
