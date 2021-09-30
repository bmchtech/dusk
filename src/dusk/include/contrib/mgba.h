#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <tonc_types.h>

#define REG_DEBUG_ENABLE (vu16*)0x4FFF780
#define REG_DEBUG_FLAGS (vu16*)0x4FFF700
#define REG_DEBUG_STRING (char*)0x4FFF600

typedef enum MGBA_LOG_LEVEL {
    MGBA_LOG_FATAL,
    MGBA_LOG_ERROR,
    MGBA_LOG_WARN,
    MGBA_LOG_INFO,
    MGBA_LOG_DEBUG
} MGBA_LOG_LEVEL;

void mgba_printf(int level, const char* ptr, ...) {
    va_list args;
    level &= 0x7;
    va_start(args, ptr);
    vsprintf(REG_DEBUG_STRING, ptr, args);
    va_end(args);
    *REG_DEBUG_FLAGS = level | 0x100;
}

BOOL mgba_open(void) {
    *REG_DEBUG_ENABLE = 0xC0DE;
    return *REG_DEBUG_ENABLE == 0x1DEA;
}

void mgba_close(void) { *REG_DEBUG_ENABLE = 0; }