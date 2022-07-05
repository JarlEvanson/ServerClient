#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <utility>

#include "common/sockets.hpp"
#include "common/packet.hpp"
#include "common/timer.hpp"
#include "common/serialize.hpp"

#include "server/server.hpp"

int main(int argv, char** argc) {
    generatePacketHeader();

    printf("Packet Header: 0x%x\n", packetHeader);

    Timer::init();

    Server server(64);

    server.run();
}

ClientDescriptor::ClientDescriptor(void) {
    memset( this, 0, sizeof( ClientDescriptor ) );
}

ClientDescriptor::ClientDescriptor(Address address, uint64_t time) {
    this->clientAddress = address;
    this->lastPacketTime = time;
}

bool ClientDescriptor::operator == (const ClientDescriptor& comp) {
    if ( this->clientAddress == comp.clientAddress )
        return true;
    return false;
}

uint64_t ClientDescriptor::getLastPacketTime(void) {
    return this->lastPacketTime;
}

Address ClientDescriptor::getAddress(void) {
    return this->clientAddress;
}

Server::Server(uint32_t maxConnections) {
    Socket::initializeSockets();

    this->maxConnections = maxConnections;
    this->currentConnections = 0;

    Socket s1 = Socket ( Address ( SERVER_SOCKET_ADDRESS ) );

    this->socket.move( s1 );

    this->clients = (ClientDescriptor*) malloc( maxConnections * sizeof(ClientDescriptor) );
    memset( clients, 0, sizeof(ClientDescriptor) * maxConnections ); 
}

void Server::run(void) {
    while ( true ) {
        uint32_t beforeCheck = getCurrentConnections();
        while ( true ) {
            sockaddr_in from;

            unsigned char packetData[256];
            unsigned int maxPacketSize = sizeof( packetData );

            int bytesRead = this->socket.receivePacket( &from, packetData, maxPacketSize );

            if ( bytesRead <= 0)
                break;

            BitReader reader( packetData, bytesRead );

            uint32_t checksum = CRC32( packetData+4, bytesRead - 4);
            uint32_t packetChecksum = reader.readBits( 32 );

            if ( memcmp( &checksum, &packetChecksum, 4 ) != 0 ) 
                continue;

            int32_t index = checkConnection( from );

            if ( index  );
        }
        pruneConnections();
        if ( beforeCheck != getCurrentConnections() ) 
            printf( "Current Connections: %u\n", getCurrentConnections() );
    }
}

int32_t Server::checkConnection(Address address) {
    for( int index = 0; index < maxConnections; index++ ) {
        if ( this->clients[index].getAddress() == address ) {
            return index;
        }
    }
    if ( currentConnections < maxConnections ) {
        return addConnection( address );
    }
    return -1;
}

void Server::pruneConnections(void) {
    uint64_t startPruneTime = Timer::getMillisecond();
    for ( int index = 0; index < maxConnections; index++ ) {
        if ( 
            this->clients[index].getLastPacketTime() + WAIT_TIME_SECS * 1000 < startPruneTime &&
            !( this->clients[index] == ClientDescriptor() )
        ) {
            memset( &this->clients[index], 0, sizeof(ClientDescriptor) );
            currentConnections--;
        }
    }
}

int32_t Server::addConnection(Address address) {
    for( int index = 0; index < maxConnections; index++ ) {
        if ( this->clients[index] == ClientDescriptor() ) {
            this->clients[index] = ClientDescriptor(address, Timer::getMillisecond());
            currentConnections++;
            return index;
        }
    }
    return -1;
}

uint32_t Server::getCurrentConnections(void) {
    return currentConnections;
}