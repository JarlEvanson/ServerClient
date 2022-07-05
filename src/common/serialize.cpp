#include <cstdlib>
#include <cstring>
#include <cmath>

#include "common/common.hpp"
#include "common/serialize.hpp"

#define BYTE_ARR_MAX 1024
#define BYTE_ARR_MIN 1
#define BYTE_ARR_RANGE ( BYTE_ARR_MAX - BYTE_ARR_MIN )

//Assumes min value is 0
constexpr uint32_t bitsRequired(uint32_t num) {
    return num > 0 ? 1 + bitsRequired(num / 2) : 0;
}

//Uint32_t alignment and sizing
BitWriter::BitWriter(void* packetBuffer, int fourByteSize, uint32_t& bytesWritten) {
    scratch = 0;
    scratchBits = 0;
    fourByteIndex = 0;
    bitsWritten = 0;
    this->fourByteSize = fourByteSize;
    this->bytesWritten = bytesWritten;
    buffer = (uint32_t*) packetBuffer;
}

void BitWriter::writeBits(uint32_t value, int bits) {
    assert(bits > 0);
    assert(bits <= 32);

    value &= (uint64_t(1) << bits) - 1;

    scratch |= value << scratchBits;

    scratchBits += bits;

    if ( scratchBits >= 32 ) {
        assert(  fourByteIndex < fourByteSize );
        #if ENDIANNESS == BIG_ENDIAN
            buffer[fourByteIndex] = BSWAP32((uint32_t)( scratch & 0xFFFFFFFF ));
        #else 
            buffer[fourByteIndex] = (uint32_t)( scratch & 0xFFFFFFFF );
        #endif
        
        scratch >>= 32;
        scratchBits -= 32;
        fourByteIndex++;
        (*this->bytesWritten) += 4;
    }

    bitsWritten += bits;
}

void BitWriter::writeFloat(float value) {
    union FloatInt {
        float fValue;
        uint32_t iValue;
    };
    FloatInt tmp;
    tmp.fValue = value;
    this->writeBits(tmp.iValue, 32);
}

constexpr float clamp(float value, float min, float max) {
    if ( value < min ) {
        return min;
    } else if ( value > max ) {
        return max;
    }
    return value;
}

void BitWriter::writeCompressedFloat(float value, float min, float max, float res) {
    const float delta = max - min;
    const float values = delta / res;
    const uint32_t maxIntegerValue = (uint32_t) ceil( values );
    const int bits = bitsRequired( maxIntegerValue );
    float normalizedValue = clamp( (value - min) / delta, 0.0f, 1.0f );
    this->writeBits( (uint32_t) floor( normalizedValue * maxIntegerValue + 0.5f ), bits );
}

//Max of 512 bytes written
void BitWriter::writeByteArray(void* str, int size) {
    assert(size >= BYTE_ARR_MIN && size <= BYTE_ARR_MAX );

    this->writeBits(size - BYTE_ARR_MIN, bitsRequired( BYTE_ARR_RANGE ));

    this->writeAlign();

    int bytesToAlignStart = 4 - ((bitsWritten / 8) % 4);

    char* const strStart = (char*) str;

    char* strPtr = strStart;

    if( bytesToAlignStart > size) {
        for(int index = 0; index < size; index++) {
            this->writeBits( *strPtr, 8 );
            strPtr++;
        }
        return;
    }

    for(int index = 0; index < bytesToAlignStart; index++) {
        this->writeBits( *strPtr, 8 );
        strPtr++;
    }

    int bytesToWriteEnd = (size - bytesToAlignStart) % 4;
    int bytesToCopy = size - bytesToAlignStart - bytesToWriteEnd;
    assert(bytesToCopy % 4 == 0);

    memcpy( this->buffer + this->fourByteIndex, strPtr, bytesToCopy );

    strPtr += bytesToCopy;
    fourByteIndex += bytesToCopy / 4;
    bitsWritten += bytesToCopy * 8;
    (*this->bytesWritten) += bytesToCopy;

    for( int index = 0; index < bytesToWriteEnd; index++ ) {
        this->writeBits( *strPtr, 8 );
        strPtr++;
    }
}

void BitWriter::writeAlign(void) {
    const int remainingBits = bitsWritten % 8;

    if ( remainingBits != 0) {
        uint32_t zero = 0;
        this->writeBits(zero, 8 - remainingBits);
        assert( (bitsWritten % 8) == 0 );
    }
}

void BitWriter::flushBits() {
    if ( scratchBits != 0 ) {
        this->writeBits(0, 32 - scratchBits);
    } 
}

int BitWriter::getBytesWritten() {
    return (bitsWritten % 32 == 0 ? bitsWritten / 32 : bitsWritten / 32 + 1) * 4;
}

unsigned char* BitWriter::getBuffer() {
    return (unsigned char*) buffer;
}

BitReader::BitReader(void* packetBuffer, int packetSize) {
    scratch = 0;
    scratchBits = 0;
    totalBits = packetSize * 8;
    totalBitsRead = 0;
    fourByteIndex = 0;
    buffer = (uint32_t*) packetBuffer;
}

uint32_t BitReader::readBits(int bits) {
    assert( bits <= 32);
    
    if ( scratchBits < bits ) {
        #if ENDIANNESS == BIG_ENDIAN
            scratch |= BSWAP32(*(buffer+fourByteIndex)) << scratchBits;
        #else
            scratch |= *(buffer+fourByteIndex) << scratchBits;
        #endif

        scratchBits += 32;
        fourByteIndex++;
    }

    uint32_t value = scratch & ((((uint64_t)1) << bits) - 1);

    scratch >>= bits;
    scratchBits -= bits;

    totalBitsRead += bits;

    return value;
}

float BitReader::readFloat() {
    union FloatInt {
        float fValue;
        uint32_t iValue;
    };
    FloatInt tmp;
    tmp.iValue = this->readBits(32);
    return tmp.fValue;
}

float BitReader::readCompressedFloat(float min, float max, float res) {
    const float delta = max - min;
    const float values = delta / res;
    const uint32_t maxIntegerValue = (uint32_t) ceil( values );
    const int bits = bitsRequired( maxIntegerValue );
    
    const float normalizedValue = this->readBits(bits) / float( maxIntegerValue );
    return normalizedValue * delta + min;
}

unsigned char* BitReader::readByteArray( int* size ) {
    *size = this->readBits( bitsRequired( BYTE_ARR_RANGE ) ) + BYTE_ARR_MIN;

    assert( *size >= BYTE_ARR_MIN && *size <= BYTE_ARR_MAX );

    if ( !this->readAlign() ) {
        *size = 0;
        return NULL;
    }

    unsigned char* const byteBuffer = (unsigned char*) malloc( *size );

    unsigned char* writePtr = byteBuffer; 

    int bytesToReadUnaligned = 4 - (totalBitsRead / 8) % 4;

    if (bytesToReadUnaligned > *size) {
        for(int index = 0; index < *size; index++) {
            *writePtr = this->readBits(8);
            writePtr++;
        }
        return byteBuffer;
    }

    for(int index = 0; index < bytesToReadUnaligned; index++) {
        *writePtr = this->readBits(8);
        writePtr++;
    }
    
    assert( totalBitsRead == fourByteIndex * 32 );

    int unalignedBytesEnd = (*size - bytesToReadUnaligned) % 4;
    int bytesToWriteAligned = *size - bytesToReadUnaligned - unalignedBytesEnd;
    assert( bytesToWriteAligned % 4 == 0 );

    memcpy( writePtr, buffer + fourByteIndex, bytesToWriteAligned );
    fourByteIndex += bytesToWriteAligned / 4;
    totalBitsRead += bytesToWriteAligned * 8;
    writePtr += bytesToWriteAligned;

    for(int index = 0; index < unalignedBytesEnd; index++) {
        *writePtr = this->readBits(8);
        writePtr++;
    }
    return byteBuffer;
}

bool BitReader::readAlign(void) {
    const int remainingBits = totalBitsRead % 8;

    if ( remainingBits != 0 ) {
        int alignValue = this->readBits(8 - remainingBits);

        assert( totalBitsRead % 8 == 0 );
        if ( alignValue != 0 ) 
            return false;
    }
    return true;
}
