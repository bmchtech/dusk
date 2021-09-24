#include <string.h>
#include "dusk.h"
#include "tonc.h"

Sprite* thred;
Anim th_anim1;

void thred_start() {
    REG_WIN0H = 0xBEB0;
    
    dusk_init_graphics_mode0();

    pal_bg_mem[0] = 0x0C02; // background color

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_thred");
    dusk_sprites_upload_atlas(&atlas);

    thred = dusk_sprites_make(0, 64, 64,
                              (Sprite){
                                  .x = SCREEN_WIDTH / 2 - 32,
                                  .y = SCREEN_HEIGHT / 2 - 32,
                                  .tid = 0,
                                  .page = 0,
                              });

    th_anim1 = MAKE_ANIM(0, 16);

    // ----------

    REG_DISPCNT |= DCNT_BG1;

    REG_WIN0H = 0xBEB1;
    tte_init_chr4c_default(1, BG_CBB(0) | BG_SBB(31));
    REG_WIN0H = 0xBEB2;
    tte_init_con();
    REG_WIN0H = 0xBEB3;

    tte_printf("thred hello");
    REG_WIN0H = 0xBEB4;
}

void thred_update() {
    REG_WIN0H = 0xCAD0;
    dusk_frame();

    dusk_sprites_anim_play(thred, &th_anim1);

    // update sprites
    dusk_sprites_update();
    REG_WIN0H = 0xCAD1;
}

void thred_end() {}

Scene thred_scene = {
    .start = thred_start,
    .update = thred_update,
    .end = thred_end,
};