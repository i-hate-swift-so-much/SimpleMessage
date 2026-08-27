#pragma once

#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "unistd.h"
#include "termios.h"
#include "stdlib.h"
#include <sys/ioctl.h>

#include "input.h"

#ifndef VER_MAJ
    #define VER_MAJ 0
#endif

#ifndef VER_MIN
    #define VER_MIN 0
#endif

extern int port;

void LogMessage(char* input);
void WinRefresh();