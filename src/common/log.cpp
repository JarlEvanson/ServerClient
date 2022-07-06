#include <cstdarg>
#include <cstdlib>
#include <cstdlib>
#include <cstdio>

#include "common/log.hpp"
#include "common/common.hpp"
#include "common/timer.hpp"


char* Logger::mFileName;
uint32_t Logger::mHandle;
bool Logger::mInitialized;
uint8_t* Logger::mBuffer;
size_t Logger::mBufferSize;

void Logger::init(const char* name, size_t bufferSize) {
    if ( !mInitialized ) {
        mFileName = (char*) name;
        mBuffer = (uint8_t*) malloc ( bufferSize );
        mBufferSize = bufferSize;
        mInitialized = true;
    }
}

void Logger::log(const char* fmt, ...) {
    va_list ap;

    printf( "[%s]: ", Timer::NPTToFormatted( Timer::getNTPTimeStamp() ) );
    
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);

    printf("\n");
}

void Logger::terminate(void) {
    if ( mInitialized ) {
        mFileName = NULL;
        free ( mBuffer );
        mBufferSize = 0;
        mInitialized = false;
    }
}