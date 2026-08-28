.PHONY: all clear

VER_MAJ := 0
VER_MIN := 1

C = gcc
CFLAGS = -Iinclude
SRC = src/main.c src/include/window.c src/include/input.c src/include/commands.c src/include/networking.c
OUT = bin/simplemsg

all:
	clear
	${MAKE} compile

compile: ${SRC}
	${C} ${CFLAGS} ${SRC} -o ${OUT} -DVER_MAJ=${VER_MAJ} -DVER_MIN=${VER_MIN}

clear:
	rm -rf bin/