#include "networking.h"

int listenSocket = -1;
int connectionSocket = -1;

bool connected = false;
bool connectionOfficial = false;

int port = 15253;

void InitNetwork(){
    connected = false;
    
    if(listenSocket > 0){
        close(listenSocket);
    }

    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(listenSocket < 0){
        LogMessage("Socket failed");
        return;
    }

    // make it so calling accept() doesn't block
    int flags = fcntl(listenSocket, F_GETFL, 0);
    if(flags == -1){
        LogMessage("Socket nonblock failed (1)");
        close(listenSocket);
        return;
    }

    if(fcntl(listenSocket, F_SETFL, flags | O_NONBLOCK) == -1){
        LogMessage("Socket nonblock failed (2)");
        close(listenSocket);
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(listenSocket, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        char reason[64];
        snprintf(reason, 64, "Bind failed (%i)", errno);
        LogMessage(reason);
        close(listenSocket);
        if(errno == EADDRINUSE){
            port++;
            InitNetwork();
        }
        return;
    }

    if(listen(listenSocket, 1) < 0){
        char reason[128];
        snprintf(reason, 128, "Listen failed (%d: %s)", errno, strerror(errno));
        LogMessage(reason);
        close(listenSocket);
        InitNetwork();
    }
}

void CheckNewConnection(){
    if(connected){return;}
    struct sockaddr received;
    socklen_t size = sizeof(struct sockaddr);

    int tempSocket = accept(listenSocket, &received, &size);
    
    if(tempSocket < 0){
        if(errno != EWOULDBLOCK && errno != EAGAIN){
            LogMessage("Accept failed");
            InitNetwork();
        }
        return;
    }

    int flags = fcntl(tempSocket, F_GETFL, 0);
    if (flags != -1) {
        fcntl(tempSocket, F_SETFL, flags | O_NONBLOCK);
    }

    connectionSocket = tempSocket;

    LogMessage("Connected from request");

    connected = true;

    SendPacket(HANDSHAKE_PACKET, 0, 0);
}

void SerializePacket(struct Packet* packet){
    packet->fullSize = htonl(packet->fullSize);
    packet->payload_length = htonl(packet->payload_length);
}

void DeSerializePacket(struct Packet* packet){
    packet->fullSize = ntohl(packet->fullSize);
    packet->payload_length = ntohl(packet->payload_length);
}

/**
 * @brief Send a packet to the current connection
 * @param type The type of packet to send
 * @param payload_length The length of the payload in bytes
 * @param payload A pointer to the payload data
 */
void SendPacket(enum PACKET_TYPES type, uint32_t payload_length, void* payload){
    if (!connected){ return; }
    struct Packet* toSend = malloc(sizeof(struct Packet)+payload_length);
    char* magic = "SMSG";
    memcpy((void*)&toSend->magic, (void*)magic, 4);
    toSend->version_major = VER_MAJ;
    toSend->version_minor = VER_MIN;
    toSend->type = type;
    toSend->payload_length = payload_length;
    toSend->fullSize = sizeof(struct Packet) + payload_length;

    memcpy((void*)toSend+sizeof(struct Packet), (void*)payload, payload_length);

    SerializePacket(toSend);

    //LogMessage("Packet sent");

    send(connectionSocket, (void*)toSend, sizeof(struct Packet)+payload_length, 0);
    free(toSend);
}

/**
 * @brief Processes a received packet
 * @param packet The packet to process
 */
void ProcessPacket(struct Packet* packet){
    switch (packet->type){
        case HANDSHAKE_PACKET:{
            if(packet->version_major != VER_MAJ){
                LogMessage("Version mismatch");
                char* disconnectMSG = "Version mismatch";

                SendPacket(DISCONNECT_PACKET, 16, disconnectMSG);
                break;
            }

            SendPacket(ACCEPT_PACKET, 0, 0);
            break;
        }

        case DISCONNECT_PACKET:{
            Disconnect();
            break;
        }

        case ACCEPT_PACKET:{
            LogMessage("Connection accepted");
            connectionOfficial = true;
            break;
        }

        case MESSAGE_PACKET:{
            char display[128];
            snprintf(display, 128, "Other: %s", packet->payload);
            LogMessage(display);
            break;
        }

        case NULL_PACKET:{
            LogMessage("Corrupted packet");
            break;
        }

        default:{
            LogMessage("Invalid packet type");
            break;
        }
    }

    free(packet);
}

void SendMessage(char* message, int messageLength){
    SendPacket(MESSAGE_PACKET, messageLength, message);
}

void CheckNewPackets(){
    if(!connected){ return; }
    // get incoming packet size
    struct Packet headerPeek;
    ssize_t packet_size = recv(connectionSocket, &headerPeek, sizeof(struct Packet), MSG_PEEK | MSG_DONTWAIT);
    if(packet_size < 0){
        if(errno != EINPROGRESS && errno != EAGAIN){
            LogMessage("Failed to receive packet size");
            return;
        }
        return;
    }
    if(packet_size == 0){
        Disconnect();
        return;
    }

    //LogMessage("Received packet");

    uint32_t given_size;
    uint32_t total = 0;

    uint32_t* temp = (uint32_t*)malloc(packet_size);

    if((total = recv(connectionSocket, temp, packet_size, 0)) < 0){
        LogMessage("Failed to receive packet");
        return;
    }else{
        given_size = ntohl(temp[0]);
    }

    struct Packet* received = malloc(given_size);

    memcpy((void*)received, (void*)temp, packet_size);

    free(temp);

    while(total < given_size){
        ssize_t n = recv(connectionSocket, received+total, given_size-total, 0);
        if(n < 0){
            LogMessage("Failed to receive packet size");
            return;
        }else if(n == 0){
            Disconnect();
            return;
        }
        total+=n;
    }

    received->payload = (uint8_t*)(sizeof(struct Packet)+received);

    DeSerializePacket(received);
    ProcessPacket(received);
}

void Disconnect(){
    if(connectionSocket >= 0){
        SendPacket(DISCONNECT_PACKET, 0, 0);
        
        close(connectionSocket);
        connectionSocket = -1;
    }

    connected = false;
    connectionOfficial = false;

    LogMessage("Disconnected");
}

bool ConnectClient(struct sockaddr_in data){
    char reason[64];

    int tempSocket = socket(AF_INET, SOCK_STREAM, 0);
    if(tempSocket < 0){return false;}

    struct pollfd pfd;
    pfd.fd = tempSocket;
    pfd.events = POLLOUT | POLLERR;

    int res = connect(tempSocket, (struct sockaddr*)&data, sizeof(data));
    if(res < 0){
        if(errno != EINPROGRESS){
            snprintf(reason, 64, "Connection failed (1) (%i)", errno);
            LogMessage(reason);
            close(tempSocket);
            return false;
        }
    }

    int poll_res = poll(&pfd, 1, 1000);

    if(poll_res <= 0){
        snprintf(reason, 64, "Connection failed (3) (%i)", errno);
        LogMessage(reason);
        close(tempSocket);
        return false;
    }

    int err = 0;
    socklen_t len = sizeof(err);
    if(getsockopt(tempSocket, SOL_SOCKET, SO_ERROR, &err, &len) < 0 || err != 0){
        snprintf(reason, 64, "Connection failed (4) (%i)", errno);
        LogMessage(reason);
        close(tempSocket);
        return false;
    }

    LogMessage("Connected");

    int flags = fcntl(tempSocket, F_GETFL, 0);
    if (flags != -1) {
        fcntl(tempSocket, F_SETFL, flags | O_NONBLOCK);
    }else{
        LogMessage("FCNTL fail");
        return false;
    }

    connected = true;

    connectionSocket = tempSocket;
    return true;
}