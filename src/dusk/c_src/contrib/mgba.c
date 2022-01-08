#include "contrib/mgba.h"

int mgba_is_open = 0;

#ifdef DEBUG
void mgba_printf(int level, const char* ptr, ...) {
    if (!mgba_is_open) return;
    va_list args;
    level &= 0x7;
    va_start(args, ptr);
    vsprintf(REG_DEBUG_STRING, ptr, args);
    va_end(args);
    *REG_DEBUG_FLAGS = level | 0x100;
}

BOOL mgba_open(void) {
    *REG_DEBUG_ENABLE = 0xC0DE;
    if (*REG_DEBUG_ENABLE == 0x1DEA) {
        mgba_is_open = 1;
        return TRUE;
    }
    return FALSE;
}

void mgba_close(void) { *REG_DEBUG_ENABLE = 0; }
#else
void mgba_printf(int level, const char* ptr, ...) {}
BOOL mgba_open(void) { return FALSE; }
void mgba_close(void) {}
#endif