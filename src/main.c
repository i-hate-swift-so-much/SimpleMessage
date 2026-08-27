#include <sys/socket.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "window.h"
#include "input.h"
#include "commands.h"
#include "networking.h"

struct termios orig_termios;

char* buffer;

// clean up the terminal for exit
void CleanUp(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    write(STDOUT_FILENO, "\033[?1049l", 10); // switch to main framebuffer
    close(listenSocket);
    close(connectionSocket);
}

int main(int argc, char* argv[]){
    // enable terminal raw mode
    tcgetattr(STDIN_FILENO, &orig_termios);
    struct termios raw = orig_termios;
    cfmakeraw(&raw);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    // alternate framebuffer
    write(STDOUT_FILENO, "\033[?1049h", 9);
    write(STDOUT_FILENO, "\033[?25l", 7); // no cursor

    // set up directories and files
    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

    mkdir("data", mode);
    int fd = open("data/contacts.txt", O_WRONLY | O_CREAT, mode);
    close(fd);
    
    InitNetwork();
    setbuf(stdout, NULL);
    WinRefresh();

    while(shouldRun){
        bool refresh = ProcessInput();

        WinRefresh();
        
        if(shouldProcessCommand){
            ProcessCommand();
        }

        CheckNewConnection();

        CheckNewPackets();
    }

    CleanUp();

    return 0;
}