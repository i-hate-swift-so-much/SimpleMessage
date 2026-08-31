# SimpleMsg Manual
This manual will show how to properly use SimpleMsg between two clients over LAN.

NOTE: SimpleMsg will not work on networks used by schools, as they often block direct LAN connection between devices.

## 1.0 Commands
SimpleMsg is used through commands, which are all documented below with their arguments. Arguments marked with a question mark (?) are denoted as optional. Arguments are enclosed in square brackets, and if the command has it, shorthands are enclosed at the end in round brackets.
### 1.1 port [port]
Sets your current clients listening port to the one speciifed. The default port is 15253.
### 1.2 connect [ip] [?port] (c)
Attempts to connect to the given IP and port on your LAN.
### 1.3 disconnect
Disconnects from the current connection.
### 1.4 message [message] (m)
Sends a message to the current connection.
### 1.5 setnick [nickname]
Sets your nickname to whatever given. If you run this command while already connected, it will not update for the connected client.

## 2.0 How to connect to another client
### 2.1 Mac
To form a connection on a Mac terminal, do the following steps.
1. Ensure you are on the same LAN as the other client.
2. Before running the program, get your current LAN IP through the command ```ipconfig getifaddr en0``` (assuming en0 is your primary network)
3. Run the program
4. Set your configuration
5. Communicate your IP and Selected port to the other person.
6. It is recommended that before establishing a connection to set a nickname through setnick [nickname] (1.5)
7. Run connect [IP] [?port] (c) (1.2) on either device
8. Send messages using message [message] (m) (1.4)
### 2.2 Linux
To form a connection on a Linux terminal, do the exact same steps, except use the command ```hostname -I``` on step 2 instead of ```ipconfig getifaddr en0``` to retrieve your LAN IP address.

## 3.0 How it works
SimpleMsg works by communcating TCP packets between two computers in a Peer to Peer esque connection. As of v1.0, these packets are NOT encrypted. Use this app at your discretion, and do not communicate sensitive info over it.
Unlike HTTP, which was the main connection inspiration, SimpleMsg uses binary based packets as opposed to character based packets, the struct is as follows.
```
struct Packet{
    uint32_t fullSize;
    uint8_t magic[4]; // SMSG
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t type;
    uint32_t payload_length;
    uint8_t* payload;
}__attribute__((packed));
```