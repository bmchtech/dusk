#include <string.h>
#include "dusk.h"

void randbit_start() {
    // set up mode 3
    dusk_init_graphics_mode3();

    sqran(0);
}

void fill_garbage() {
    for (int x = 0; x < M3_WIDTH; x++) {
        for (int y = 0; y < M3_HEIGHT; y++) {
            u32 rnd = qran();
            u16 col = (rnd & 0xFFFF);
            vid_mem[y * M3_WIDTH + x] = col;
        }
    }
}

void randbit_update() {
    fill_garbage();

    dusk_frame();
}

void randbit_end() {}

Scene randbit_scene = {
    .start = randbit_start,
    .update = randbit_update,
    .end = randbit_end,
};