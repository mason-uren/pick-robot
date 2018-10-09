#!/bin/bash

IP_ADDRESS=ip_data.txt

binaries=( \
	../pick-robot/Debug/pick-robot \
	../pick-trigger-app/Debug/pick-trigger-app \
)

while read line;
do
	if ! [[ $line = '#'* ]] ;
	then
		scp ${binaries[@]} pi@${line}:~/
	fi
done < ${IP_ADDRESS}
