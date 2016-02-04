#!/bin/sh
if [ ! -d build-mingw32 ]; then
	mkdir build-mingw32
fi
cd build-mingw32
cmake -DCMAKE_TOOLCHAIN_FILE=../Toolchain-cross-mingw32-linux.cmake ..
make
