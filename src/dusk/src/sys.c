#include <tonc.h>
#include "ds_sys.h"
#include "ds_load.h"

uint frame_count;

void dusk_init_all() {
    // 1. initialize system graphics
    dusk_init_graphics();

    // initialize loader
    dusk_load_init();
}

void dusk_init_graphics() {
    // mode 0
    REG_DISPCNT = DCNT_MODE0;
}

void dusk_frame() {
    vid_vsync();
    frame_count++;
}

static bool scene_changed = false;
static void nothing(void) {}
static Scene next_scene = {
    .start = nothing,
    .end = nothing,
    .update = nothing,
};
static Scene current_scene = {
    .start = nothing,
    .end = nothing,
    .update = nothing,
};

void dusk_scene_set(Scene next) {
    scene_changed = true;
    next_scene = next;
}

void dusk_scene_update() {
    if (scene_changed) {
        scene_changed = false;
        current_scene.end();
        current_scene = next_scene;
        current_scene.start();
    }
    current_scene.update();
}
