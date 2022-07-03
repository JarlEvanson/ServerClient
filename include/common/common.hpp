#ifndef GAME_COMMON
#define GAME_COMMON

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <assert.h>

#define BSWAP32(value) \
    ((((value) >> 24) & 0xFF) | (((value) & 0xFF) << 24) | (((value) >> 8) & 0xFF00) | (((value) << 8) & 0xFF0000))

#endif