#include <stdio.h>
#include <string.h>

#include "common/sockets.hpp"
#include "common/packet.hpp"
#include "common/serialize.hpp"


int main(int argv, char** argc) {
    generatePacketHeader();

    printf("Packet Header: 0x%x\n", packetHeader);

    Address addr("127.0.0.1:2000");
    //Initialize Sockets

    //Create and bind socket

    Socket::initializeSockets();

    Socket socket(addr);

    unsigned char buffer[256];

    memset( buffer, 0, sizeof( buffer ) );

    BitWriter writer ( &buffer[4], (sizeof(buffer) / 4) - 1 );

    writer.writeBits(0x12345678, 32);

    writer.flushBits();

    uint32_t crc = CRC32( &buffer[4], writer.getBytesWritten() );

    printf("%u\n", crc);

    writer = BitWriter ( buffer, sizeof( buffer ) / 4 );

    writer.writeBits( crc, 32 );

    writer.writeBits(0x12345678, 32);

    writer.flushBits();

    Address toAddr("127.0.0.1:2050");

    socket.sendTo(toAddr, (const char*) writer.getBuffer(), writer.getBytesWritten());
}