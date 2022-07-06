#ifndef GAME_TIMER
#define GAME_TIMER

#include "common/common.hpp"

#if PLATFORM == PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winnt.h>
#elif PLATFORM == PLATFORM_MAC
    
#elif PLATFORM == PLATFORM_UNIX
    #include <unistd.h>
#endif


class Timer {
    static bool initialized;
    #if PLATFORM == PLATFORM_WINDOWS
        static uint64_t freq;
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