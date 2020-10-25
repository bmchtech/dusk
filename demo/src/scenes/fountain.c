#include <string.h>
#include "dusk.h"
#include "contrib/gbamap.h"

Map fnt_map;
Sprite* eggcat;
Anim walk;
const int SHIFT_SPEED = 1;
BackgroundPoint bg_shift;

void fountain_start() {
    dusk_init_graphics();
    
    // set up background
    fnt_map = dusk_load_map("fountain");
    map_init_registers();
    map_set_onscreen(fnt_map);

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_chars");
    dusk_sprites_upload_atlas(&atlas);

    // make eggcat sprite
    eggcat = dusk_sprites_make(0, 16, 16,
        (Sprite){
            .x = SCREEN_WIDTH / 2 - 8,
            .y = SCREEN_HEIGHT / 2 - 8,
            .tid = 8,
            .page = 0,
        });
    walk = MAKE_ANIM(0, 4);

    bg_shift = (BackgroundPoint){128, 248};
}

void fountain_update() {
    dusk_frame();

    // input
    int y_move = key_tri_vert();
    int x_move = key_tri_horz();
    bool moving = (y_move != 0 || x_move != 0);

    // update bg scroll
    bg_shift.y += y_move * SHIFT_SPEED;
    bg_shift.x += x_move * SHIFT_SPEED;

    if (moving)
        dusk_sprites_anim_play(eggcat, &walk);
    else
        eggcat->page = 0;

    // update map position
    map_shift(fnt_map, bg_shift);

    // update sprites
    dusk_sprites_update();
}

void fountain_end() {}

Scene fountain_scene = {
    .start = fountain_start,
    .update = fountain_update,
    .end = fountain_end,
};