#ifndef GAME_TIMER
#define GAME_TIMER

#include "common/common.hpp"

typedef union _NPT_TimeStamp {
    struct {
        uint32_t seconds;
        uint32_t fractionalSeconds;
    } seperated;
    uint64_t timeStamp;
} NPTTimeStamp;

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
        static NPTTimeStamp getNTPTimeStamp(void);
        static char* NPTToFormatted(NPTTimeStamp timeStamp);
};

#endif