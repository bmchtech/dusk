#include "contrib/mgba.h"

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