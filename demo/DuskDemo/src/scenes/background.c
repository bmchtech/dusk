#include <string.h>
#include "dusk.h"
#include "tonc.h"

Sprite* background;

void background_start() {
    dusk_init_graphics_mode0();

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_background");
    dusk_sprites_upload_atlas(&atlas);

    background = dusk_sprites_make(0, 64, 64,
                              (Sprite){
                                  .x = SCREEN_WIDTH / 2 - 32,
                                  .y = SCREEN_HEIGHT / 2 - 32,
                                  .tid = 0,
                                  .page = 0,
                              });

    // ----------

    // REG_DISPCNT |= DCNT_BG1;
    // tte_init_chr4c(1, BG_CBB(0)|BG_SBB(31), 0, 0x0201, CLR_GRAY, NULL, NULL);
    // tte_init_con();

    // tte_printf("#{P:12,12}rendered 3d");
    // tte_printf("#{P:200,140}next >");
}

void background_update() {
    dusk_frame();

    // update sprites
    dusk_sprites_update();
}

void background_end() {}

Scene background_scene = {
    .start = background_start,
    .update = background_update,
    .end = background_end,
};