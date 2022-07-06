#include <stdio.h>
#include <string.h>

#include "common/sockets.hpp"
#include "common/packet.hpp"
#include "common/serialize.hpp"
#include "common/timer.hpp"


int main(int argv, char** argc) {
    Timer::init();
    printf( "Starting at %s\n", Timer::NPTToFormatted( Timer::getNTPTimeStamp() ) );

    generatePacketHeader();
    //Initialize Sockets

    //Create and bind socket

    Socket::initializeSockets();

    Socket socket( "127.0.0.1:2000" );

    Packet packet(256);

    BitWriter& writer = packet.getBitWriter();

    writer.writeBits(0x12345678, 32);

    writer.flushBits();

    packet.finalize();

    Address toAddr("127.0.0.1:2050");

    socket.sendTo(toAddr, packet);
}