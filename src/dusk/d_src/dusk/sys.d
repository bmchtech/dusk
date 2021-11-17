module dusk.sys;

extern(C):

import core.stdc.stdio;
import tonc;
import dusk.load;

struct Scene {
    void function() start;
    void function() end;
    void function() update;
}

// /** initialize the system for using DUSK */
// void dusk_init_all();
// /** initialize mode0 graphics */
// void dusk_init_graphics_mode0();
// /** initialize mode3 graphics */
// void dusk_init_graphics_mode3();
// /** vsync */
// void dusk_frame();
// void dusk_scene_set(Scene next);
// void dusk_scene_update();

// __attribute__((used)) const char* _DUSK_LIB_VERSION = ("$DUSK " DUSK_VERSION);
// const char* _DUSK_LIB_VERSION = ("$DUSK " ~ DUSK_VERSION);

__gshared static uint frame_count;

void dusk_clear_vidmem() {
    // clear video memory (vram region)
    memset32(vid_mem, 0, VRAM_BG_SIZE / 4);
    // pal mem
    memset32(pal_bg_mem, 0, 0x00200 / 4);
    memset32(pal_obj_mem, 0, 0x00200 / 4);
}

void dusk_reset_irq() {
    irq_init(null);
    irq_add(eIrqIndex.II_VBLANK, null);
}

void dusk_init_all() {
    // 1. initialize system graphics
    dusk_init_graphics_mode0();

    dusk_reset_irq();

    // initialize loader
    dusk_load_init();
}

void dusk_init_graphics_mode0() {
    dusk_clear_vidmem();
    // dusk_reset_irq();

    // reset registers
    *REG_DISPCNT = DCNT_MODE0;
    *REG_BG0CNT = 0;
    *REG_BG1CNT = 0;
    *REG_BG2CNT = 0;
    *REG_BG3CNT = 0;
}

void dusk_init_graphics_mode3() { *REG_DISPCNT = DCNT_MODE3 | DCNT_BG2; }

void dusk_frame() {
    // vid_vsync();
    VBlankIntrWait();
    frame_count++;
}

__gshared static bool scene_changed = false;
__gshared static void nothing() {}

__gshared static Scene next_scene = Scene(&nothing, &nothing, &nothing);
__gshared static Scene current_scene = Scene(&nothing, &nothing, &nothing);

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
