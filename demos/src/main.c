#include <string.h>
#include <tonc.h>
#include "contrib/gbamap.h"
#include "dusk.h"

int main() {
    dusk_init();

    // set up background
    Map map = dusk_load_map("fountain");
    map_init_registers();
    map_set_onscreen(map);

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atlas0");
    dusk_sprites_upload_atlas(&atlas);

    // make eggcat sprite
    Sprite* eggcat = dusk_sprites_make(0, 16, 16,
        (Sprite){
            .x = SCREEN_WIDTH / 2 - 8,
            .y = SCREEN_HEIGHT / 2 - 8,
            .tid = 8,
            .page = 0,
        });
    Anim walk = (Anim){.start = 0, .len = 4};

    const int SHIFT_SPEED = 1;
    BackgroundPoint bg_shift = {128, 248};
    while (TRUE) {
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
}