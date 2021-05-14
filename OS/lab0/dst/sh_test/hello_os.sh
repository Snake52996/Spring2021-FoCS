#!/bin/bash
lines=(8 32 128 512 1024)
echo -n > "$2"
for l in ${lines[@]}
do
	head -n $l "$1" | tail -n 1 >> "$2"
done
