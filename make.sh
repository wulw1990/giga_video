#!/bin/bash

mkdir -p build
cd build

if [ $# -eq 0 ]; then
	cmake ..
	make -j16
	echo "make -j16 end"
else
	if [ $# -eq 1 -a "$1"=="clean" ]; then
		# echo $1
		cd ..
		rm -rf build
		# make clean
		echo "make clean end"
	fi
fi
