#include <string.h>
#include "dusk.h"
#include "tonc.h"

Sprite* me_spr1;
Anim me_anim1;

void merge_start() {
    dusk_init_graphics_mode0();

    pal_bg_mem[0] = 0x0C02; // background color

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_thred");
    dusk_sprites_upload_atlas(&atlas);

    me_spr1 = dusk_sprites_make(0, 64, 64,
                              (Sprite){
                                  .x = SCREEN_WIDTH / 2 - 32,
                                  .y = SCREEN_HEIGHT / 2 - 32,
                                  .tid = 0,
                                  .page = 0,
                              });

    me_anim1 = MAKE_ANIM(0, 16);

    // ----------

    REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, BG_CBB(0)|BG_SBB(31), 0, 0x0201, CLR_GRAY, NULL, NULL);
    tte_init_con();

    tte_printf("#{P:12,12}merge");
    tte_printf("#{P:200,140}next >");
}

void merge_update() {
    dusk_frame();

    dusk_sprites_anim_play(me_spr1, &me_anim1);

    // update sprites
    dusk_sprites_update();
}

void merge_end() {}

Scene merge_scene = {
    .start = merge_start,
    .update = merge_update,
    .end = merge_end,
};