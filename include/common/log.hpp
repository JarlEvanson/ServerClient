#ifndef GAME_LOGGER
#define GAME_LOGGER

#include <filesystem>

class Logger {  
    static std::filesystem::path mDir;
    static char* mFileName;
    static uint32_t mHandle;
    static bool mInitialized;
    static uint8_t* mBuffer;
    static size_t mBufferSize;
    public:
        static void init(const char* name, size_t bufferSize);
        static void log(char* msg);
        static void terminate(void);
};

#endif