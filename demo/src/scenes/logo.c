#include <string.h>
#include "dusk.h"

Sprite* logo;

void logo_start() {
    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atlas_logo_0");
    dusk_sprites_upload_atlas(&atlas);

    // make eggcat sprite
    logo = dusk_sprites_make(0, 64, 64,
        (Sprite){
            .x = SCREEN_WIDTH / 2 - 32,
            .y = SCREEN_HEIGHT / 2 - 32,
            .tid = 0,
            .page = 0,
        });
}

void logo_update() {
    dusk_frame();

    // input
    key_poll();

    // update sprites
    dusk_sprites_update();
}

void logo_end() {}

Scene logo_scene = {
    .start = logo_start,
    .update = logo_update,
    .end = logo_end,
};