#!/bin/bash

vendors=(LinuxCgSDK LinuxOpenGL LinuxSDL)

for i in "${vendors[@]}"; do
	echo Unpacking vendor ${i}.7z
	#rm -rf ${i}
	7za x -y ${i}.7z
done

