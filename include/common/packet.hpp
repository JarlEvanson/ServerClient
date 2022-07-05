#ifndef GAME_PACKET
#define GAME_PACKET

#include "common/common.hpp"
#include "common/sockets.hpp"

#define MAX_PACKET_SIZE 1200

extern uint32_t packetHeader;

void generatePacketHeader(void);

class Packet {
    unsigned char* packetData;
    bool finalized;  
    uint32_t bytesUsed;
    uint32_t maxBytes;
    BitReader& reader;
    BitWriter& writer;
    public:
        Packet(uint32_t maxPacketSize);
        Packet(unsigned char* packetData, uint32_t dataLen);
        ~Packet();
        uint32_t calculateCRC32(void);
        void finalize(void);
};

class PacketTracker {
    Address address;
    uint32_t localSequenceNumber;
    uint32_t localAckBitField;
    uint32_t remoteSequenceNumber;
    public:
        int sendPacket(Socket& socket, void* data, uint32_t dataLen);
};

#endif