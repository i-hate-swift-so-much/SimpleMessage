#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "input.h"
#include "window.h"
#include "networking.h"

enum COMMAND_IDENT{
    CONNECT,
    DISCONNECT,
    BLOCK,
    SETWHITELIST,
    ADDWHITELIST,
    CONTACTS,
    ADDCONTACT,
    UPDATECONTACT
};

struct Command{
    enum COMMAND_IDENT Ident;
    void* args[16];
    uint8_t argCount;
};

struct BlockDesignator{
    char* block;
    char* shortBlock;
    size_t bytes;
};

void SetNickname(char* nick, size_t nickLen);
void ProcessCommand();