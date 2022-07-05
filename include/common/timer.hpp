#ifndef GAME_TIMER
#define GAME_TIMER

#include "common/common.hpp"

#if PLATFORM == PLATFORM_WINDOWS
    #include <winnt.h>
#elif PLATFORM == PLATFORM_MAC
    
#elif PLATFORM == PLATFORM_UNIX
    #include <unistd.h>
#endif


class Timer {
    static bool initialized;
    #if PLATFORM == PLATFORM_WINDOWS
        static LARGE_INTEGER freq;
    #elif PLATFORM == PLATFORM_UNIX
        static bool usingMonotonic;
    #elif PLATFORM == PLATFORM_MAC
        static uint64_t freqNumerator;
        static uint64_t freqDenominator;
    #endif

    public:
        static void init(void);
        static uint64_t getMillisecond(void);

};

#endif