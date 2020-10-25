#include <string.h>
#include "dusk.h"

SpriteAtlasLayout atlas_layout;

void autumn_start() {
    dusk_init_graphics();
    
    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_part");
    dusk_sprites_upload_atlas(&atlas);

    // load atlas layout
    atlas_layout = dusk_load_atlas_layout("atl_part");

    // get an entry and make a sprite
    SpriteAtlasEntry* leaf_entry = dusk_load_atlas_entry(&atlas_layout, "lvs10");
    u16 leaf_tid = dusk_sprites_pos_to_tid(leaf_entry->x, leaf_entry->y, atlas_layout.width, atlas_layout.height);
    Sprite* leaf = dusk_sprites_make(0, leaf_entry->w, leaf_entry->h,
        (Sprite){
            .x = 20,
            .y = 100,
            .tid = leaf_tid,
        });
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