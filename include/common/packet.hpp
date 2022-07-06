#ifndef GAME_PACKET
#define GAME_PACKET

#include "common/common.hpp"
#include "common/sockets.hpp"
#include "common/serialize.hpp"

class Address;
class Socket;

#define MAX_PACKET_SIZE 1200

extern uint32_t packetHeader;

void generatePacketHeader(void);

class Packet {
    unsigned char* packetData;
    bool finalized;  
    bool shouldFreeData;
    uint32_t bytesUsed;
    uint32_t maxBytes;
    bool isBitWriter;
    void* bitPacker;
    public:
        Packet(uint32_t maxPacketSize);
        Packet(unsigned char* packetData, uint32_t dataLen);
        ~Packet();
        uint32_t calculateCRC32(void);
        BitWriter& getBitWriter(void);
        BitReader& getBitReader(void);
        void finalize(void);

    friend class Socket;
};

class Address;

/*
class PacketTracker {
    Address address;
    uint32_t localSequenceNumber;
    uint32_t localAckBitField;
    uint32_t remoteSequenceNumber;
    public:
        int sendPacket(Socket& socket, void* data, uint32_t dataLen);
}; */

#endif