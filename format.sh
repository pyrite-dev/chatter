#!/bin/sh
while [ ! -d .git ]; do
	cd ..
done
clang-format --verbose -i `find src include load.c -name "*.c" -or -name "*.h"`
