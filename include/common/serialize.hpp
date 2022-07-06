#ifndef GAME_SERIALIZE
#define GAME_SERIALIZE

#include "common/common.hpp"

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
        void writeFloat(float value);
        void writeCompressedFloat(float value, float min, float max, float res);
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
        float readFloat();
        float readCompressedFloat(float min, float max, float res);
        unsigned char* readByteArray( int* size );
        bool readAlign(void);
};

class Serializable {
    virtual void serialize(BitWriter& writer) = 0;
    virtual void deserialize(BitReader& reader) = 0;
};


#endif