#include <string.h>
#include "dusk.h"
#include "tonc.h"

Sprite* thred;
Anim anim1;

void thred_start() {
    dusk_init_graphics_mode0();

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

    anim1 = MAKE_ANIM(0, 16);

    // ----------

    REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, BG_CBB(0)|BG_SBB(31), 0, 0x0201, CLR_GRAY, NULL, NULL);
    tte_init_con();

    tte_printf("#{P:12,12}rendered 3d");
    tte_printf("#{P:200,140}next >");
}

void thred_update() {
    dusk_frame();

    dusk_sprites_anim_play(thred, &anim1);

    // update sprites
    dusk_sprites_update();
}

void thred_end() {}

Scene thred_scene = {
    .start = thred_start,
    .update = thred_update,
    .end = thred_end,
};