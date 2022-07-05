#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>

#include "common/sockets.hpp"

Address::Address(void) {
    memset( this, 0, sizeof( Address ) );
}

Address::Address(unsigned short port) {
    memset( this, 0, sizeof ( Address ) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
}

Address::Address(unsigned char a, unsigned char b, unsigned char c, unsigned char d, unsigned short port) {
    unsigned int address = (a << 24) | (b << 16) | (c << 8) | d;

    memset( this, 0, sizeof ( Address ) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( address );
    addr.sin_port = htons( port );
}

Address::Address(const char* addressString) {
    char* string = (char*) malloc(strlen(addressString));
    memcpy(string, addressString, strlen(addressString));
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

    memset( this, 0, sizeof ( Address ) );
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl( address );
    addr.sin_port = htons( intPort );
}

Address::Address(sockaddr_in address) {
    memset( this, 0, sizeof ( Address ) );
    addr = address;
}

int Address::toIntAddress() {
    return ntohl(addr.sin_addr.s_addr);
}

int Address::toPort() {
    return ntohs(addr.sin_port);
}

char* Address::toString() {
    int intAddress = this->toIntAddress();
    int port = this->toPort();

    char* string = (char*) malloc(21); //15 for ipv4 address, 6 for port

    sprintf(string, "%u.%u.%u.%u:%u", intAddress >> 24, intAddress >> 16 & 255, intAddress >> 8 & 255, intAddress & 255, port);

    return string;
}

bool Address::operator==(const Address& addr) {
    if ( this->addr.sin_addr.s_addr == addr.addr.sin_addr.s_addr )
        return true;
    return false;
}

bool Socket::sendTo(Address address, const char* packetData, int packetLen) {
    int sentBytes = sendto(
        handle, 
        (const char*) packetData, 
        packetLen, 
        0, 
        (sockaddr*) &address, 
        sizeof(sockaddr_in) 
    );

    if ( sentBytes != packetLen ) {
        printf("Failed to send packet\n");
        return false;
    }
    return true;
}

Socket::Socket() {
    handle = -1;
}

void Socket::move(Socket& source) {
    this->handle = source.handle;
    source.handle = -1;
}

Socket::Socket(Address address) {
    if( !initHandle() ) {
        exit(-1);
    }

    this->bindPort(address);
}

bool Socket::bindPort(Address address) {
    if ( bind( handle, (const sockaddr*) &address, sizeof(sockaddr_in) ) < 0 ) {
        printf("Failed to bind socket\n");
        return false;
    }
    return true;
}

bool Socket::initHandle() {
    handle = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );

    if ( handle <= 0 ) {
        printf("Failed to create socket\n");
        return false;
    }

    #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
        int nonBlocking = 1;
        if ( fcntl( handle, F_SETFL, O_NONBLOCK, nonBlocking ) == -1 )  {
            printf( "Failed to set non-blocking\n" );
            return false;
        }

    #elif PLATFORM == PLATFORM_WINDOWS
        DWORD nonBlocking = 1;
        if ( ioctlsocket( handle, FIONBIO, &nonBlocking ) != 0 ) {
            printf( "Failed to set non-blocking\n" );
            return false;
        }
    #endif
    return true;
}

bool Socket::initializeSockets() {
    #if PLATFORM == PLATFORM_WINDOWS
        WSADATA WsaData;
        if ( !(WSAStartup( MAKEWORD(2, 2), &WsaData ) == NO_ERROR) ) {
            printf("Failed to initialize sockets\n");
            return false;
        }
    #endif
    return true;
}

void Socket::shutdownSockets() {
    #if PLATFORM == PLATFORM_WINDOWS
    WSACleanup();
    #endif
}

int Socket::receivePacket(
    sockaddr_in* from,
    void* packetData, 
    unsigned int maxPacketSize
) {
    #if PLATFORM == PLATFORM_UNIX || PLATFORM == PLATFORM_MAC
        __socklen_t fromLength = sizeof(sockaddr_in);
    #elif PLATFORM == PLATFORM_WINDOWS
        int fromLength = sizeof(sockaddr_in);
    #endif
    int bytes = recvfrom( 
        this->handle, 
        (char*) packetData, 
        maxPacketSize, 
        0,
        (sockaddr*) from,
        &fromLength
    );

    return bytes;
}

Socket::~Socket() {
    if ( handle != -1 ) {
        #if PLATFORM == PLATFORM_MAC || PLATFORM == PLATFORM_UNIX
            close( this->handle );
        #elif PLATFORM == PLATFORM_WINDOWS
            closesocket( this->handle );
        #endif
    }
}