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

    const char* nameString = "Jarl Evanson";

    writer.writeByteArray( (void*)nameString, 12 );

    writer.writeBits(10, 16);

    writer.writeFloat(10.01f);

    writer.writeCompressedFloat(2.521f, 0.0f, 10.0f, 0.001);

    writer.flushBits();

    Address toAddr("127.0.0.1:2050");

    socket.sendTo(toAddr, (const char*) writer.getBuffer(), writer.getBytesWritten());
}