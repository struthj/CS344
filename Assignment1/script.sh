#!/bin/bash
while [[$ -gt 1]]
do
key="$1"
case $key in
	-r* )
	FILENAME="$2"
	CALC="Row"
	shift
	;;
	-c* )
	FILENAME="2"
	CALC="Col"
	shift
	;;
esac
shift
done
echo FILENAME
echo CALC