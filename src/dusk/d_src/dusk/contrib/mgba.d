module dusk.contrib.mgba;

import tonc.tonc_types;

extern (C):

enum REG_DEBUG_ENABLE = cast(vu16*) 0x4FFF780;
enum REG_DEBUG_FLAGS = cast(vu16*) 0x4FFF700;
enum REG_DEBUG_STRING = cast(char*) 0x4FFF600;

enum MGBALogLevel
{
    FATAL = 0,
    ERROR = 1,
    WARN = 2,
    INFO = 3,
    DEBUG = 4
}

__gshared extern int mgba_is_open;
void mgba_printf (int level, const(char)* ptr, ...);
bool mgba_open ();
void mgba_close ();
