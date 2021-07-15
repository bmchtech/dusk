#include <string.h>
#include "dusk.h"
#include "tonc.h"

Sprite* logo;
const int FADE_LENGTH = 60; // fade length in frames
int start_frame;
int fade_step;

void logo_start() {
    dusk_init_graphics_mode0();

    start_frame = frame_count;

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_logo");
    dusk_sprites_upload_atlas(&atlas);

    logo = dusk_sprites_make(0, 64, 64,
                             (Sprite){
                                 .x = SCREEN_WIDTH / 2 - 32,
                                 .y = SCREEN_HEIGHT / 2 - 32,
                                 .tid = 0,
                             });

    // enable blend on this object
    OBJ_ATTR* logo_attr = &obj_buffer[0];
    obj_set_attr(logo_attr, logo_attr->attr0 | ATTR0_BLEND, logo_attr->attr1, logo_attr->attr2);

    // set up blending registers
    REG_BLDCNT = BLD_OBJ | BLD_BLACK;
    REG_BLDY = BLDY_BUILD(16);

    fade_step = FADE_LENGTH / 16;

    // ----------

	REG_DISPCNT |= DCNT_BG0;
	tte_init_chr4c(0, BG_CBB(0)|BG_SBB(31), 0, 0x0201, CLR_WHITE, NULL, NULL);
	tte_init_con();

    printf("dusk demo");
}

void logo_update() {
    dusk_frame();

    int progress = (frame_count - start_frame);
    if (progress <= FADE_LENGTH) {
        int fade = clamp(progress / fade_step, 0, 16);
        REG_BLDY = BLDY_BUILD(16 - fade);
    }

    // update sprites
    dusk_sprites_update();
}

void logo_end() {
    // clear blending registers
    REG_BLDCNT = BLD_OFF;
}

Scene logo_scene = {
    .start = logo_start,
    .update = logo_update,
    .end = logo_end,
};