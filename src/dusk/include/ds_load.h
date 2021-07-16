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

#define ATLAS_ENTRY_LEN 6

/** represents a single entry in the sprite atlas.
 * note that names are limited to just 5 chars and a NULL terminator. */
typedef struct SpriteAtlasEntry {
    char name[ATLAS_ENTRY_LEN];
    // all of these are in tiles (8px)
    u8 x;
    u8 y;
    u8 w;
    u8 h;
} SpriteAtlasEntry;

/** represents the layout of items in an atlas (https://github.com/xdrie/crunch#binary-format).
 * we only use the first texture because the GPU can only hold one */
typedef struct SpriteAtlasLayout {
    u16 width;
    u16 height;
    u16 num_entries;
    SpriteAtlasEntry* entries;
} SpriteAtlasLayout;

/** initialize loader */
void dusk_load_init();

// - CONTENT LOADERS: pass filename without extension

SpriteAtlas dusk_load_atlas(char* name);
SpriteAtlasLayout dusk_load_atlas_layout(const char* name);
SpriteAtlasEntry* dusk_load_atlas_entry(SpriteAtlasLayout* layout, const char* entry_name);
void dusk_free_atlas_layout(SpriteAtlasLayout* layout);

Map dusk_load_map(char* name);