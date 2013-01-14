#!/bin/bash

vendors=(LinuxCgSDK LinuxOpenGL LinuxSDL)

for i in "${vendors[@]}"; do
	echo Packing vendor ${i}.7z
	#rm -f ${i}.7z
	7za a ${i}.7z "${i}"
done

