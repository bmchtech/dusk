/*
    DUSK: LOADER
*/

#pragma once

#include "contrib/gbfs.h"

extern const GBFS_FILE *gbfs_dat;

typedef struct SpriteAtlas {
    u32* tiles;
    u32 tile_sz;
    u32* pal;
    u32 pal_sz;
} SpriteAtlas;

void dusk_load_init(); // initialize loader
SpriteAtlas dusk_load_atlas(char* filename);