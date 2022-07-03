#include <stdio.h>


#include "common/sockets.hpp"
#include "common/packet.hpp"

int main(int argv, char** argc) {
    generatePacketHeader();

    printf("Packet Header: 0x%x\n", packetHeader);

    Address addr("127.0.0.1:2050");

    Socket::initializeSockets();

    Socket socket(addr);
    
    while ( true ) {
        while ( true ) {
            sockaddr_in from;

            unsigned char packetData[256];
            unsigned int maxPacketSize = sizeof(packetData);

            int bytesRead = socket.receivePacket(&from, packetData, maxPacketSize);

            if ( bytesRead <= 0) {
                break;
            }

            BitReader reader(packetData, bytesRead);

            if ( reader.readBits(32) != packetHeader ) {
                continue;
            }

            printf("%s: ", ((Address)from).toString() );

            printf("0x%x\n", reader.readBits(32));

            int strLen;

            char* str = (char*) reader.readByteArray( &strLen );

            printf("%u: %.*s\n", strLen, strLen, str);
        }  
    }
}