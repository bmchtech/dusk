#include <string.h>
#include "dusk.h"
#include "tonc.h"

void background_start() {
    dusk_init_graphics_mode0();

    // load bg
    GritImage bg_img = dusk_load_image("dusk_bg");
    dusk_background_upload_raw(&bg_img, 0, 30);

    dusk_background_make(0, (Background){.x = 0, .y = 0, .cbb = 0, .sbb = 30});
}

void background_update() { dusk_frame(); }

void background_end() {}

Scene background_scene = {
    .start = background_start,
    .update = background_update,
    .end = background_end,
};