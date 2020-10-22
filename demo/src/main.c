#include <gba_console.h>
#include <gba_video.h>
#include <gba_interrupt.h>
#include <gba_systemcalls.h>
#include <gba_input.h>
#include <stdio.h>
#include <stdlib.h>

char beef(char* g) { 
    return g[0];
}

int main() {
    char* str_a = "apples";
    char* str_b = "beans";
    char* str_c = "cakes";

    char a = 'x';
    char b = 'x';
    char c = 'x';

    a = beef(str_a);
    b = beef(str_b);
    c = beef(str_c);

    // the vblank interrupt must be enabled for VBlankIntrWait() to work
	// since the default dispatcher handles the bios flags no vblank handler
	// is required
	irqInit();
	irqEnable(IRQ_VBLANK);

	consoleDemoInit();

	// ansi escape sequence to set print co-ordinates
	// /x1b[line;columnH
	iprintf("\x1b[10;10HTest:\n a=%d, b=%d, c=%d\n", a, b, c);

	while (1) {
		VBlankIntrWait();
	}
}