#include <string.h>
#include "dusk.h"

SpriteAtlasLayout atlas_layout;
const int NUM_LEAVES = 32;
const int GRAVITY = 1;

SpriteAtlasEntry* pick_random_leaf() {
    int entry_ix = qran_range(0, atlas_layout.num_entries);
    SpriteAtlasEntry* entry = &atlas_layout.entries[entry_ix];
    return entry;
}

Sprite* respawn_leaf(int i, int x, int y) {
    SpriteAtlasEntry* leaf_entry = pick_random_leaf();
    Sprite* leaf = dusk_sprites_make(
        i, leaf_entry->w, leaf_entry->h,
        (Sprite){
            .x = x,
            .y = y,
            .tid = dusk_sprites_pos_to_tid(leaf_entry->x, leaf_entry->y, atlas_layout.width, atlas_layout.height),
        });
    return leaf;
}

void autumn_start() {
    dusk_init_graphics();

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_part");
    dusk_sprites_upload_atlas(&atlas);

    // load atlas layout
    atlas_layout = dusk_load_atlas_layout("atl_part");

    // // get an entry and make a sprite
    // SpriteAtlasEntry* leaf_entry = dusk_load_atlas_entry(&atlas_layout, "lvs10");
    // u16 leaf_tid = dusk_sprites_pos_to_tid(leaf_entry->x, leaf_entry->y, atlas_layout.width, atlas_layout.height);
    // Sprite* leaf = dusk_sprites_make(0, leaf_entry->w, leaf_entry->h,
    //     (Sprite){
    //         .x = 20,
    //         .y = 100,
    //         .tid = leaf_tid,
    //     });

    // set up N leaf sprites
    for (int i = 0; i < NUM_LEAVES; i++) {
        int ix = qran_range(0, SCREEN_WIDTH);
        int iy = qran_range(0, SCREEN_HEIGHT);
        respawn_leaf(i, ix, iy);
    }
}

void autumn_update() {
    dusk_frame();

    // update leaves
    for (int i = 0; i < NUM_LEAVES; i++) {
        // respawn if out of range
        if (sprites[i].y > SCREEN_HEIGHT) {
            int ix = qran_range(0, SCREEN_WIDTH);
            int iy = 0;
            respawn_leaf(i, ix, iy);
        }
        // apply gravity
        int grav_roll = qran_range(0, 3);
        if (grav_roll == 0) {
            sprites[i].y += GRAVITY;
        }
    }

    // update sprites
    dusk_sprites_update();
}

void autumn_end() { dusk_free_atlas_layout(&atlas_layout); }

Scene autumn_scene = {
    .start = autumn_start,
    .update = autumn_update,
    .end = autumn_end,
};