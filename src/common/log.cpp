#include "common/log.hpp"

#include "common/common.hpp"

std::filesystem::path Logger::mDir;
char* Logger::mFileName;
uint32_t Logger::mHandle;
bool Logger::mInitialized;
uint8_t* Logger::mBuffer;
size_t Logger::mBufferSize;

void Logger::init(const char* name, size_t bufferSize) {
    if ( !mInitialized ) {
        mFileName = (char*) name;
        mDir = std::filesystem::current_path();
        mBuffer = (uint8_t*) malloc ( bufferSize );
        mBufferSize = bufferSize;
        mInitialized = true;
    }
}

void Logger::log(char* msg) {
    
}

void Logger::terminate(void) {
    if ( mInitialized ) {
        mFileName = NULL;
        mDir = std::filesystem::path();
        free ( mBuffer );
        mBufferSize = 0;
        mInitialized = false;
    }
}