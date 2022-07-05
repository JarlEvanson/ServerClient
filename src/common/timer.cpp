
#include "common/common.hpp"
#include "common/timer.hpp"

#if PLATFORM == PLATFORM_UNIX
    #include <time.h>
    #include <sys/time.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
    LARGE_INTEGER Timer::freq;
#elif PLATFORM == PLATFORM_UNIX
    bool Timer::usingMonotonic;
#elif PLATFORM == PLATFORM_MAC
    uint64_t Timer::freqNumerator;
    uint64_t Timer::freqDenominator;
#endif

bool Timer::initialized;

void Timer::init(void) {
    if ( initialized ) {
        return;
    }
    #if PLATFORM == PLATFORM_WINDOWS
        LARGE_INTEGER tcounter;
        
        if ( QueryPerformanceFrequency( &tcounter ) != 0 ) {
            freq = tcounter.QuadPart;
            Timer::initialized = true;
            return;
        }
        Timer::initialized = false;
    #elif PLATFORM == PLATFORM_UNIX
        #ifdef _SC_MONOTONIC_CLOCK
            if ( sysconf( _SC_MONOTONIC_CLOCK ) > 0 ) {
                Timer::usingMonotonic = true; 
                Timer::initialized = true;
                return;
            } else {
                Timer::usingMonotonic = false;
                Timer::initialized = true;
                return;
            }
        #endif
        Timer::initialized = false;
    #elif PLATFORM == PLATFORM_MAC 
        mach_timebase_info_data tb;
        
        if ( mach_timebase_info_data ( &tb ) == KERN_SUCCESS  && tb.denom != 0 ){
            Timer::freqNumerator = (uint64_t) tb.numer;
            Timer::freqDenominator = (uint64_t) tb.denom;
            Timer::initialized = true;
            return;
        }
        Timer::initialized = false;        
    #endif
}

uint64_t Timer::getMillisecond(void) {
    #if PLATFORM == PLATFORM_WINDOWS
        LARGE_INTEGER tcounter;
        
        if ( QueryPerformanceCounter( &tcounter ) != 0 ) {
            return tcounter.QuadPart;
        } else {
            return 0;
        }
        return (uint64_t) (ticks / ( freq / 1000000 ));
    #elif PLATFORM == PLATFORM_UNIX
        if ( Timer::usingMonotonic ) {
            struct timespec ts;

            if ( clock_gettime( CLOCK_MONOTONIC, &ts ) == 0 ) {
                return (uint64_t) (ts.tv_sec * 1000 + ts.tv_nsec / 1000000 );
            } else {
                return 0;
            }
        } else {
            struct timeval tv;

            if (gettimeofday( &tv, NULL ) == 0)
                return (uint64_t) (tv.tv_sec * 1000000 + tv.tv_usec);
            else {
                return 0;
            }
        }
    #elif PLATFORM == PLATFORM_MAC
        uint64_t tickValue = mach_absolute_time();

        tickValue /= 1000;

        tickValue *= Timer::freqNumerator;
        tickValue /= Timer::freqDenominator;
    #endif
}