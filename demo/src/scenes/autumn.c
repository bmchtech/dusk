#include <string.h>
#include "dusk.h"

SpriteAtlasLayout atlas_layout;

void autumn_start() {
    dusk_init_graphics();

    REG_DISPCNT |= DCNT_BG1; // enable bg1

    // --- (1) Base TTE init for tilemaps ---
    tte_init_se(1,                      // Background number (BG 1)
                BG_CBB(0) | BG_SBB(31), // BG control (for REG_BGxCNT)
                4,                      // Tile offset (special cattr)
                CLR_PURPLE,             // Ink color
                14,                     // BitUnpack offset (on-pixel = 15)
                &sys8Font,         // Default font (sys8)
                NULL);                  // Default renderer (se_drawg_s)

    // --- (3) Print some text ---

    tte_write("press key!\n");

    tte_write("bap!\n");

    // load sprite atlas
    // dusk_sprites_init();
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG1 | DCNT_OBJ | DCNT_OBJ_1D;
    SpriteAtlas atlas = dusk_load_atlas("atl_part");
    dusk_sprites_upload_atlas(&atlas);

    // load atlas layout
    atlas_layout = dusk_load_atlas_layout("atl_part");

    char tst[64];
    sprintf(tst, " n: %d\n", atlas_layout.num_entries);
    tte_write(tst);

    // get an entry and make a sprite
    // SpriteAtlasEntry* leaf_entry = dusk_load_atlas_entry(&atlas_layout, "lvs10");
    SpriteAtlasEntry* leaf_entry = dusk_load_atlas_entry(&atlas_layout, "lvs5");
    // u16 leaf_tid = leaf_entry->x / 8;
    u16 leaf_tid = 1;
    Sprite* leaf = dusk_sprites_make(0, leaf_entry->w, leaf_entry->h,
        (Sprite){
            .x = 20,
            .y = 100,
            .tid = leaf_tid,
        });
    sprintf(tst, " w: %d, h: %d, tid: %d\n", leaf_entry->w, leaf_entry->h, leaf_tid);
    tte_write(tst);
}

void autumn_update() {
    dusk_frame();

    // update sprites
    dusk_sprites_update();
}

void autumn_end() { dusk_free_atlas_layout(&atlas_layout); }

Scene autumn_scene = {
    .start = autumn_start,
    .update = autumn_update,
    .end = autumn_end,
};