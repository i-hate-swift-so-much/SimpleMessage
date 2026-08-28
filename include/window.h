#pragma once

#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "unistd.h"
#include "termios.h"
#include "stdlib.h"
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netdb.h>

#include "input.h"

#ifndef VER_MAJ
    #define VER_MAJ 0
#endif

#ifndef VER_MIN
    #define VER_MIN 0
#endif

extern int port;

extern char connectedIP[INET6_ADDRSTRLEN];
extern int conPort;
extern char nickname[32];
extern int nicknameLength;

extern char connectNickname[32];
extern int connectedNickLen;

void LogMessage(char* input);
void WinRefresh();