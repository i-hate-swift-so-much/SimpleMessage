#include "window.h"

int width = 0;
int height = 0;

void PlaceChar(char c, int x, int y){
    printf("\033[%i;%iH%c", x, y, c);
}

void MoveCursor(int x, int y){
    printf("\033[%i;%iH", y + 1, x + 1);
}

int logPos = 0;
char messageLog[128][128];

void LogMessage(char* input){
    snprintf(messageLog[logPos], sizeof(messageLog[logPos]), "%s", input);
    logPos++;

    if(logPos == height-1 || logPos == 127){
        logPos = 0;
    }

    WinRefresh();
}

void WinRefresh(){
    struct winsize ws;
    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0){
        height = ws.ws_row;
        width = ws.ws_col;
    }

    MoveCursor(0, 0);
    printf("\033[Kv%i.%i | %i | %i", VER_MAJ, VER_MIN, inputBufferPos, port);

    MoveCursor(0, height);
    printf("\033[K> %s", inputBuffer);

    for(int i = 0; i < logPos; i++){
        MoveCursor(0, i+1);
        printf("%s\n", messageLog[i]);
    }
    fflush(stdout);
}