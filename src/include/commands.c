#include "commands.h"

bool CompareStrings(char* string0, size_t size0, char* string1, size_t size1){
    if(size0 != size1){
        return false;
    }
    for(int i = 0; i < size0; i++){
        if(string0[i] != string1[i]){
            return false;
        }
    }
    return true;
}

int FirstSpace(char* str, int len){
    for(int i = 0; i < len; i++){
        if(str[i] == ' '){
            return i;
        }
    }

    return 0;
}

void DumpContacts(){
    char* contactFile = "data/contacts.txt";

    LogMessage("CONTACTS LIST:");

    struct stat st;
    stat(contactFile, &st);

    char* fileBuffer = (char*)malloc(st.st_size);

    int contact_fd = open(contactFile, O_RDONLY);
    read(contact_fd, fileBuffer, st.st_size);

    char curContact[64];
    memset(curContact, 0, 64);
    int contactPos = 0;
    int i;
    for(i = 0; i < st.st_size; i++){
        if(fileBuffer[i] == '\n'){
            curContact[63] = 0;
            LogMessage(curContact);
            memset(curContact, 0, 64);
            contactPos = 0;
        }else if(contactPos >= 64){
            curContact[63] = 0;
            LogMessage(curContact);
            memset(curContact, 0, 64);
            contactPos = 0;
        }else{
            curContact[contactPos] = fileBuffer[i];
            contactPos++;
        }
    }

    curContact[63] = 0;
    LogMessage(curContact);

    free(fileBuffer);
    close(contact_fd);

    LogMessage("END OF CONTACTS");
}

int GetArgIndex(int arg){
    int spaces = 0;
    for(int i = 0; i < inputBufferPos; i++){
        if(inputBuffer[i] == ' '){
            spaces++;
        }
        if(spaces == arg){
            return i+1;
        }
    }
    return -1;
}

void ProcessCommand(){
    shouldProcessCommand = false;

    // practically, this just creates a list of BlockDesignators, each part of a command gets it's own.
    // space separated
    int blockCount = 1;

    char tempBuffer[128];
    memcpy(tempBuffer, inputBuffer, 128);

    for(int i = 0; i < inputBufferPos; i++){
        if(inputBuffer[i] == ' '){
            tempBuffer[i] = '\0';
            blockCount++;
        }
    }

    struct BlockDesignator* blocks = malloc(sizeof(struct BlockDesignator) * blockCount);
    memset(blocks, 0, sizeof(struct BlockDesignator) * blockCount);

    int lastBlockPos = 0;
    for(int i = 0; i < blockCount; i++){
        blocks[i].block = (char*)&inputBuffer[lastBlockPos];
        blocks[i].shortBlock = (char*)&tempBuffer[lastBlockPos];
        for(int c = lastBlockPos; c < inputBufferPos; c++){
            blocks[i].bytes = c-lastBlockPos;
            if(tempBuffer[c] == '\0'){
                break;
            }
        }
        lastBlockPos+= blocks[i].bytes+1;

        /*
        char debug[64];
        snprintf(debug, 64, "Size=%zu", blocks[i].bytes);
        LogMessage(debug);
        snprintf(debug, 64, "%s", blocks[i].block);
        LogMessage(debug);
        */
    }

    if(CompareStrings(blocks[0].shortBlock, blocks[0].bytes, "contacts", 8)){
        DumpContacts();
    }else if(CompareStrings(blocks[0].shortBlock, blocks[0].bytes, "port", 4)){
        if(blockCount != 2){
            LogMessage("Err: No port provided");
            return;
        }
        char* portIndex = blocks[1].shortBlock;
        port = strtol(portIndex, NULL, 10);
        char msg[128];
        snprintf(msg, 128, "Set port to %i", port);
        LogMessage(msg);
        InitNetwork();
    }else if(CompareStrings(blocks[0].shortBlock, blocks[0].bytes, "connect", 7) || CompareStrings(blocks[0].shortBlock, blocks[0].bytes, "c", 1)){
        struct sockaddr_in data;
        data.sin_family = AF_INET;
        if(blockCount == 3){
            data.sin_port = htons(strtol(blocks[2].shortBlock, NULL, 10));
        }else if(blockCount == 2){
            data.sin_port = htons(15253);
        }else{
            LogMessage("Err: No address included");
            return;
        }

        if(inet_pton(AF_INET, blocks[1].shortBlock, (void*)&data.sin_addr) <= 0){
            LogMessage("IP Conversion Failure");
            return;
        }

        ConnectClient(data);
    }else if(CompareStrings(blocks[0].shortBlock, blocks[0].bytes, "disconnect", 10)){
        Disconnect();
    }else if(CompareStrings(blocks[0].shortBlock, blocks[0].bytes, "message", 7) || CompareStrings(blocks[0].shortBlock, blocks[0].bytes, "m", 1)){
        char* messageIndex = blocks[1].block;
        if(messageIndex != NULL){
            SendMessage(messageIndex, inputBufferPos-blocks[0].bytes);
            char display[128];
            snprintf(display, 128, "You: %s", messageIndex);
            LogMessage(display);
        }else{
            LogMessage("Couldn't send message: NO ARG");
        }
    }else{
        LogMessage("Invalid Command");
        return;
    }
    memset(inputBuffer, 0, 128);

    inputBufferPos = 0;
}