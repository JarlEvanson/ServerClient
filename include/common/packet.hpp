#ifndef GAME_PACKET
#define GAME_PACKET

#include "common/common.hpp"

extern uint32_t packetHeader;

void generatePacketHeader(void);

constexpr uint32_t bitsRequired(uint32_t num);

class BitWriter {
    private:
        uint64_t scratch;
        int scratchBits;
        int fourByteIndex;
        int fourByteSize;
        int bitsWritten;
        uint32_t* buffer;
    public:
        BitWriter(void* packetBuffer, int sizeWords);
        void writeBits(uint32_t  value, int bits);
        void writeByteArray(void* str, int size);
        void writeAlign(void);
        void flushBits();
        int getBytesWritten();
        unsigned char* getBuffer();
};

class BitReader {
    private:
        uint64_t scratch;
        int scratchBits;
        int totalBits;
        int totalBitsRead;
        int fourByteIndex;
        uint32_t* buffer;
    public:
        BitReader(void* packetBuffer, int packetSize);
        uint32_t readBits(int bits);
        unsigned char* readByteArray( int* size );
        bool readAlign(void);
};

#endif