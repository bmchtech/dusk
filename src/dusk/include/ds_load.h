/*
    DUSK: LOADER
*/

#pragma once

#include "contrib/gbfs.h"
#include "contrib/gbamap.h"

extern const GBFS_FILE* gbfs_dat;

// represents an atlas, with bitmaps for the image and palette
typedef struct SpriteAtlas {
    u32* tiles;
    u32 tile_sz;
    u32* pal;
    u32 pal_sz;
} SpriteAtlas;

// represents a single entry in the sprite atlas.
// note that names are limited to just 5 chars and a NULL terminator.
typedef struct SpriteAtlasEntry {
    char name[6];
    u8 x;
    u8 y;
    u8 w;
    u8 h;
} SpriteAtlasEntry;

// represents the layout of items in an atlas (https://github.com/xdrie/crunch#binary-format)
// we only use the first texture because the GPU can only hold one
typedef struct SpriteAtlasLayout {
    u16 num_entries;
    SpriteAtlasEntry* entries;
} SpriteAtlasLayout;

void dusk_load_init(); // initialize loader

// - CONTENT LOADERS: pass filename without extension

SpriteAtlas dusk_load_atlas(char* name);
SpriteAtlasLayout dusk_load_atlas_layout(char* name);
void dusk_free_atlas_layout(SpriteAtlasLayout* layout);

Map dusk_load_map(char* name);