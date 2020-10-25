#include <string.h>
#include "dusk.h"

Map atm_map;
SpriteAtlasLayout atlas_layout;
const int GRAVITY = 1;

typedef struct LeafInfo {
    s8 fall_points;
    u8 max_fall_points;
    u8 side_stage;
    u8 side_prd;
    s8 slant;
    u8 slant_prd;
    u16 col;
} LeafInfo;

#define NUM_LEAVES 32
#define SIDE_STAGE_SZ 35

const s8 SIDE_STAGES[SIDE_STAGE_SZ] = {5,  5,  5,  4,  4,  3,  3,  2, 1, 0, -1, -2, -3, -3, -4, -4, -5, -5,
                                       -5, -4, -4, -3, -3, -2, -1, 0, 1, 2, 3,  3,  4,  4,  5,  5,  5};
LeafInfo leaves[NUM_LEAVES];

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

    leaves[i].max_fall_points = qran_range(0, 4);
    leaves[i].side_prd = qran_range(2, 5);
    leaves[i].col = x;
    leaves[i].side_stage = qran_range(0, SIDE_STAGE_SZ);
    sprites[i].x = leaves[i].col + SIDE_STAGES[leaves[i].side_stage];

    // 30% to have slant
    leaves[i].slant = 0;
    if (qran_range(0, 10) <= 2) {
        leaves[i].slant = qran_range(-1, 2);
        leaves[i].slant_prd = qran_range(1, 3);
    }

    return leaf;
}

void autumn_start() {
    dusk_init_graphics();

    // set up background
    atm_map = dusk_load_map("september");
    map_init_registers();
    map_set_onscreen(atm_map);

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_part");
    dusk_sprites_upload_atlas(&atlas);

    // load atlas layout
    atlas_layout = dusk_load_atlas_layout("atl_part");

    // set up N leaf sprites
    for (int i = 0; i < NUM_LEAVES; i++) {
        int ix = qran_range(0, SCREEN_WIDTH);
        int iy = qran_range(0, SCREEN_HEIGHT);
        respawn_leaf(i, ix, iy);
    }

    // update map position
    BackgroundPoint bg_shift = (BackgroundPoint){0, 0};
    map_shift(atm_map, bg_shift);
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
        // - motion

        // 1. update fall points
        leaves[i].fall_points--;
        if (leaves[i].fall_points < 0) {
            leaves[i].fall_points = qran_range(0, leaves[i].max_fall_points);
            sprites[i].y += GRAVITY;
            if ((frame_count % leaves[i].slant_prd) == 0) {
                leaves[i].col += leaves[i].slant;
            }
        }
        // 2. update side stages
        if ((frame_count % leaves[i].side_prd) == 0) {
            leaves[i].side_stage = (leaves[i].side_stage + 1) % SIDE_STAGE_SZ;
            sprites[i].x = leaves[i].col + SIDE_STAGES[leaves[i].side_stage];
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