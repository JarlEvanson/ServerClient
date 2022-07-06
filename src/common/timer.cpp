
#include "common/common.hpp"
#include "common/timer.hpp"

#include <chrono>
#include <stdio.h>
#include <stdlib.h>


#if PLATFORM == PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <winnt.h>
    #include <sysinfoapi.h>
#elif PLATFORM == PLATFORM_MAC
    
#elif PLATFORM == PLATFORM_UNIX
    #include <unistd.h>
    #include <time.h>
    #include <sys/time.h>
#endif

#if PLATFORM == PLATFORM_WINDOWS
    uint64_t Timer::freq;
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
            Timer::freq = (uint64_t) tcounter.QuadPart;
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
        
        uint64_t ticks;
        if ( QueryPerformanceCounter( &tcounter ) != 0 ) {
            ticks = (uint64_t) tcounter.QuadPart;
        } else {
            ticks = 0;
        }
        return (uint64_t) ( ticks / ( Timer::freq / 1000000 ) );
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
        return tickValue;
    #endif
}


//Ignores leap seconds
NPTTimeStamp Timer::getNTPTimeStamp(void) {
    uint64_t msSinceUnixEpoch; //Unix epoch is January 1, 1970
    #if PLATFORM == PLATFORM_UNIX
        msSinceUnixEpoch = std::chrono::duration_cast<std::chrono::milliseconds>( std::chrono::system_clock::now().time_since_epoch() ).count();
    #elif PLATFORM == PLATFORM_WINDOWS
        FILETIME ft;
        GetSystemTimeAsFileTime( &ft );
        uint64_t msSinceWindowsEpoch =  ( (uint64_t)ft.dwLowDateTime + ((uint64_t)(ft.dwHighDateTime) << 32) ) / 10000; //Windows Epoch is January 1, 1601

        msSinceUnixEpoch = msSinceWindowsEpoch - (uint64_t) 11644473600000; //11644473600000 is ms between Windows Epoch and Unix Epoch
    #endif
    NPTTimeStamp timeStamp;
    timeStamp.seperated.seconds = msSinceUnixEpoch / 1000 + ( (uint32_t)2208988800 );
    timeStamp.seperated.fractionalSeconds = ((( (uint64_t)msSinceUnixEpoch ) % 1000 ) << 32) / 1000;
    return timeStamp;
}

//Ignores leap seconds
char* Timer::NPTToFormatted(NPTTimeStamp timeStamp) {
    uint64_t sSinceUnixEpoch = 
        ( ( (uint64_t)(timeStamp.seperated.seconds) - 2208988800 ) * 1000 +
        ((( (uint64_t)timeStamp.seperated.fractionalSeconds ) * 1000) >> 32) ) / 1000;

    uint32_t day = sSinceUnixEpoch / ( 24 * 60 * 60 ) + 1;
    uint32_t year = 1970;

    uint8_t monthArr[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    while( day > 365 ) {
        day -= 365;
        if ( year % 4 == 0 ) {
            if ( year % 100 != 0 || year % 400 == 0 ) {
                day -= 1;
                if ( day < 365 ) {
                    monthArr[1]++;
                }
            }
        }
        year++;
    }

    uint8_t monthIndex = 0;
    while ( day > monthArr[monthIndex] ) {
        day -= monthArr[monthIndex];
        monthIndex++;
    }

    char* str = (char*) malloc( 21 );

    uint32_t secondsToday = sSinceUnixEpoch % ( 24 * 60 * 60 );

    sprintf( str, "%.4u-%.2u-%.2uT%.2u:%.2u:%.2uZ", year, monthIndex + 1, day, (secondsToday / (60 * 60)), (secondsToday % (60 * 60)) / 60, secondsToday % 60 );

    return str;
}