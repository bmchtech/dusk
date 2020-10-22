#include <string.h>
#include "dusk.h"
#include "contrib/gbamap.h"

Map map;
Sprite* eggcat;
Anim walk;
const int SHIFT_SPEED = 1;
BackgroundPoint bg_shift;

void start() {
    // set up background
    map = dusk_load_map("fountain");
    map_init_registers();
    map_set_onscreen(map);

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atlas_chars_0");
    dusk_sprites_upload_atlas(&atlas);

    // make eggcat sprite
    eggcat = dusk_sprites_make(0, 16, 16,
        (Sprite){
            .x = SCREEN_WIDTH / 2 - 8,
            .y = SCREEN_HEIGHT / 2 - 8,
            .tid = 8,
            .page = 0,
        });
    walk = (Anim){.start = 0, .len = 4};

    bg_shift = (BackgroundPoint){128, 248};
}

void update() {
    dusk_frame();

    // input
    key_poll();
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
    map_shift(map, bg_shift);

    // update sprites
    dusk_sprites_update();
}

void end() {}

Scene fountain_scene = {
    .start = start,
    .update = update,
    .end = end,
};