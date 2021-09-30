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

void mgba_printf(int level, const char* ptr, ...);
BOOL mgba_open(void);
void mgba_close(void);