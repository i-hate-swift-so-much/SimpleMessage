.PHONY: all clear

VER_MAJ := 0
VER_MIN := 1

BREW_PREFIX := $(shell brew --prefix)

C = gcc
CFLAGS = -Iinclude -I$(BREW_PREFIX)/include
LDFLAGS = -L$(BREW_PREFIX)/lib -lgmp
SRC = src/main.c src/include/window.c src/include/input.c src/include/commands.c src/include/networking.c
OUT = bin/simplemsg

all:
	clear
	${MAKE} compile

compile: ${SRC}
	${C} ${CFLAGS} ${SRC} -o ${OUT} ${LDFLAGS} -DVER_MAJ=${VER_MAJ} -DVER_MIN=${VER_MIN}

clear:
	rm -rf bin/