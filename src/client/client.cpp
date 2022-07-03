#include <stdio.h>


#include "common/sockets.hpp"
#include "common/packet.hpp"


int main(int argv, char** argc) {
    generatePacketHeader();

    printf("Packet Header: 0x%x\n", packetHeader);

    Address addr("127.0.0.1:2000");
    //Initialize Sockets

    //Create and bind socket

    Socket::initializeSockets();

    Socket socket(addr);

    unsigned char buffer[256];

    BitWriter writer ( buffer, sizeof(buffer) / 4 );

    writer.writeBits(packetHeader, 32);

    writer.writeBits(0x12345678, 32);

    char* nameString = "Jarl Evanson";

    writer.writeByteArray( (void*)nameString, 12 );

    writer.flushBits();

    BitReader reader(buffer, writer.getBytesWritten());

    reader.readBits(32);

    reader.readBits(32);

    int strLen;

    reader.readByteArray( &strLen );

    Address toAddr("127.0.0.1:2050");

    socket.sendTo(toAddr, (const char*) writer.getBuffer(), writer.getBytesWritten());
}