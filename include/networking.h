#pragma once

#include "stdbool.h"
#include "stdio.h"
#include "stdint.h"
#include "unistd.h"
#include "termios.h"
#include "stdlib.h"
#include <sys/ioctl.h>
#include "sys/socket.h"
#include <netinet/in.h>
#include "fcntl.h"
#include <arpa/inet.h>
#include <string.h>
#include <poll.h>
#include <netdb.h>

#include "window.h"

extern int listenSocket;
extern int connectionSocket;

extern int port;
extern bool connected;

void InitNetwork();
void CheckNewConnection();
void Disconnect();

void SendMessage(char* message, int messageLength);

enum PACKET_TYPES{
    NULL_PACKET,
    HANDSHAKE_PACKET,
    ACCEPT_PACKET,
    DISCONNECT_PACKET,
    MESSAGE_PACKET
};

struct Packet{
    uint32_t fullSize;
    uint8_t magic[4]; // Always equals "SMSG"
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t type;
    uint32_t payload_length;
    uint8_t* payload;
}__attribute__((packed));

/*
    Connect to another client with TCP
*/
bool ConnectClient(struct sockaddr_in data);

void SendPacket(enum PACKET_TYPES type, uint32_t payload_length, void* payload);
void CheckNewPackets();