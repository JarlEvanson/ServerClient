#ifndef GAME_COMMON
#define GAME_COMMON

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define PLATFORM_WINDOWS 1
#define PLATFORM_MAC 2
#define PLATFORM_UNIX 3

#if defined(_WIN32) 
    #define PLATFORM PLATFORM_WINDOWS
#elif defined(__APPLE__)
    #define PLATFORM PLATFORM_MAC
#else
    #define PLATFORM PLATFORM_UNIX
#endif

#define BSWAP32(value) \
    ((((value) >> 24) & 0xFF) | (((value) & 0xFF) << 24) | (((value) >> 8) & 0xFF00) | (((value) << 8) & 0xFF0000))

#endif