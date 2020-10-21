#ifndef VIDEO_H
#define VIDEO_H

#include "./memoryMap.h"

/**
 *  Defines general video control.
 */

typedef u16 Color;

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 160

#define REGISTER_DISPLAY_CONTROL *(vu32*)(REGISTER_BASE+0x0000)
#define REGISTER_VERTICAL_COUNT  *(vu16*)(REGISTER_BASE+0x0006)

#define FLAG_MODE0 0

/**
 * Wait till the background has been rendered and it is safe to modify it.
 **/
INLINE void videoSync()
{
	while(REGISTER_VERTICAL_COUNT >= 160);
	while(REGISTER_VERTICAL_COUNT < 160);
}

#endif // VIDEO_H
