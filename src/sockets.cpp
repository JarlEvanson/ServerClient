#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sockets.hpp"


bool initalizeSockets() {
    #if PLATFORM == PLATFORM_WINDOWS
    WSADATA WsaData;
    return WSAStartup(MAKEWORD(2, 2), &WsaData);
    #else
    return true;
    #endif
}

void shutdownSockets() {
    #if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
    #endif
}

int main(int argv, char** argc) {


    Address obj;

    char* str = "127.0.0.1:2050";

    char* str1 = (char*) malloc(15);

    memccpy(str1, str, 0, 15);

    obj.convertAddressFromIP(str1);
    //Initialize Sockets

    if ( !initalizeSockets() ) {
        printf("Failed to initialize sockets\n");
    }

    //Create and bind socket

    int handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if ( handle <= 0 ) {
        printf("Failed to create socket\n");
    }
    int port = 0; 

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( (unsigned short) port);

    if ( bind(handle, (const sockaddr*) &address, sizeof(sockaddr_in)) < 0) {
        printf("Failed to bind socket\n");
        return false;
    }

    //Enable non-blocking

    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX 
    int nonBlocking = -1;
        if ( fcntl(handle, F_SETFL, O_NONBLOCK, nonBlocking) == -1 ) {
            printf("Failed to set non-blocking\n");
        } 
    #elif PLATFORM == PLATFORM_WINDOWS
    DWORD nonBlocking = 1;
        if ( ioctlsocket( handle, FIONBIO, &nonBlocking) != 0 ) {
            printf("Failed to set non-blocking\n");
        }
    #endif

    //Send packets

    int packetData = 0;
    int packetSize = 0;

    int sent_bytes = sendto(handle, (const char*) packetData, packetSize, 0, (sockaddr*) &address, sizeof(sockaddr_in) );

    if ( sent_bytes != packetSize ) {
        printf("Failed to send packet\n");
    }
}

Address::Address(char* string) {
    char* a = strtok(string, ".");
    unsigned int intA = atoi(a);
    char* b = strtok(NULL, ".");
    unsigned int intB = atoi(b);
    char* c = strtok(NULL, ".");
    unsigned int intC = atoi(c);
    char* d = strtok(NULL, ":");
    unsigned int intD = atoi(d);
    char* port = strtok(NULL, ":");
    unsigned int intPort = atoi(port);

    unsigned int address = (intA << 24) | (intB << 16) | (intC << 8) | intD;

    sockaddr_in addr_in;
    addr_in.sin_family = AF_INET;
    addr_in.sin_addr.s_addr = htonl( address );
    addr_in.sin_port = htons( intPort );

    addr = addr_in;
}

sockaddr Address::toSockAddr() {
    return addr;
}


