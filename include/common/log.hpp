#ifndef GAME_LOGGER
#define GAME_LOGGER

#include "common/common.hpp"

class Logger {
    static char* mFileName;
    static uint32_t mHandle;
    static bool mInitialized;
    static uint8_t* mBuffer;
    static size_t mBufferSize;
    public:
        static void init(const char* name, size_t bufferSize);
        static void log(const char* fmt, ...);
        static void terminate(void);
};

#endif