module dusk.contrib.mgba;

import tonc.tonc_types;

extern (C):

enum REG_DEBUG_ENABLE = cast(vu16*) 0x4FFF780;
enum REG_DEBUG_FLAGS = cast(vu16*) 0x4FFF700;
enum REG_DEBUG_STRING = cast(char*) 0x4FFF600;

enum MGBA_LOG_LEVEL
{
    MGBA_LOG_FATAL = 0,
    MGBA_LOG_ERROR = 1,
    MGBA_LOG_WARN = 2,
    MGBA_LOG_INFO = 3,
    MGBA_LOG_DEBUG = 4
}

void mgba_printf (int level, const(char)* ptr, ...);
bool mgba_open ();
void mgba_close ();
