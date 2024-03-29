#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <tonc_types.h>
#include "ds_load.h"
#include <stdlib.h>
#include <stdio.h>

const GBFS_FILE* gbfs_dat;

void dusk_load_init() {
    // locate the main GBFS archive
    gbfs_dat = find_first_gbfs_file(find_first_gbfs_file);
}

const void* dusk_load_raw(char* name, u32* len) {
    return gbfs_get_obj(gbfs_dat, name, len);
}

GritImage dusk_load_image(char* name) {
    GritImage img;
    char tiles_name[GBFS_ENTRY_SIZE];
    strcpy(tiles_name, name);
    strcat(tiles_name, ".img.bin");
    char pal_name[GBFS_ENTRY_SIZE];
    strcpy(pal_name, name);
    strcat(pal_name, ".pal.bin");
    char map_name[GBFS_ENTRY_SIZE];
    strcpy(map_name, name);
    strcat(map_name, ".map.bin");

    img.tiles = (u32*)gbfs_get_obj(gbfs_dat, tiles_name, &img.tile_sz);
    img.pal = (u32*)gbfs_get_obj(gbfs_dat, pal_name, &img.pal_sz);
    img.map = (u32*)gbfs_get_obj(gbfs_dat, map_name, &img.map_sz);

    return img;
}

SpriteAtlas dusk_load_atlas(char* name) {
    char atlas_name[GBFS_ENTRY_SIZE];
    strcpy(atlas_name, name);
    strcat(atlas_name, "_0");

    GritImage img = dusk_load_image(atlas_name);
    
    SpriteAtlas atlas;
    atlas.tiles = img.tiles;
    atlas.tile_sz = img.tile_sz;
    atlas.pal = img.pal;
    atlas.pal_sz = img.pal_sz;

    // 1. pal_sz is some weird gradient thing
    // we're going to detect when more than 4 in the row are the same
    // so then find the actual end of the palette

    // NOTE: ASSERT pal_sz >= 2

    u16* pal_raw = (u16*)atlas.pal; // 16-bit color
    const int palscan_start = 0;
    const int palscan_inarow = 6; // how many in a row we're looking for

    u16 prev_color = pal_raw[palscan_start];
    int true_pal_sz = 2;                     // true size of palette (starts at 1 color, so 2 bytes)
    int col_match_streak = 0;                // number of last few colors that matched
    int last_color_index = atlas.pal_sz / 2; // last color (div by 2 cause pal_sz is in bytes, and 16-bit color)

    for (int i = palscan_start + 1; i < last_color_index; i++) { // start at 1 cause 0's in prev_color
        u16 pal_col = pal_raw[i];                                // color at index
        // printf("p: %0x\n", (int)pal_col);

        // check for match
        if (pal_col == prev_color) {
            col_match_streak++;
        } else {
            col_match_streak = 0;
        }

        if (col_match_streak >= palscan_inarow) {             // 4 past colors are same
            true_pal_sz = true_pal_sz - (palscan_inarow) * 2; // we'll disregard all the last 4
            break;                               // done
        }

        prev_color = pal_col;
        true_pal_sz += 2;
    }
    atlas.pal_sz = true_pal_sz; // set adjusted pal size
    // printf("adjpal: %d\n", true_pal_sz);

    return atlas;
}

void seek_until_null(const u8* data, int* pos) {
    char last = data[*pos];
    while (last != 0) {
        *pos = *pos + 1; // advance pointer
        last = data[*pos];
    }
}

SpriteAtlasLayout dusk_load_atlas_layout(const char* name) {
    SpriteAtlasLayout layout;
    char file_name[GBFS_ENTRY_SIZE];
    strcpy(file_name, name);
    strcat(file_name, "_.sht.bin");

    u32 data_sz = 0;
    const u8* data = gbfs_get_obj(gbfs_dat, file_name, &data_sz);

    int pos = 0;

    // get the number of textures
    int num_tex = data[pos] | (data[pos + 1] << 8);
    assert(num_tex == 1); // there should only be a single texture
    pos += 2;
    // eat the texture name
    seek_until_null(data, &pos);
    pos++;

    layout.width = data[pos] | (data[pos + 1] << 8);
    pos += 2;
    layout.height = data[pos] | (data[pos + 1] << 8);
    pos += 2;

    layout.num_entries = data[pos] | (data[pos + 1] << 8);
    layout.entries = malloc(sizeof(SpriteAtlasEntry) * layout.num_entries);
    pos += 2;

    // now read blocks
    for (int i = 0; i < layout.num_entries; i++) {
        // save the entry name
        char last = data[pos];
        int name_len = 0;
        while (last != 0) {
            layout.entries[i].name[name_len] = last;
            name_len++;
            last = data[pos + name_len];
        }
        layout.entries[i].name[name_len] = '\0'; // null terminator
        pos += name_len + 1;                     // skip the name

        u16 x = data[pos] | (data[pos + 1] << 8);
        pos += 2;
        u16 y = data[pos] | (data[pos + 1] << 8);
        pos += 2;
        u16 w = data[pos] | (data[pos + 1] << 8);
        pos += 2;
        u16 h = data[pos] | (data[pos + 1] << 8);
        pos += 2;
        // x,y,w,h are 8-bit so don't overflow!
        // these are in tile units (the final div by 8)
        layout.entries[i].x = x / 8;
        layout.entries[i].y = y / 8;
        layout.entries[i].w = w / 8;
        layout.entries[i].h = h / 8;
    }

    return layout;
}

SpriteAtlasEntry* dusk_load_atlas_entry(SpriteAtlasLayout* layout, const char* entry_name) {
    for (int i = 0; i < layout->num_entries; i++) {
        SpriteAtlasEntry* entry = &layout->entries[i];
        if (strncmp(entry_name, entry->name, ATLAS_ENTRY_LEN) == 0) {
            return entry;
        }
    }
    return NULL;
}

void dusk_free_atlas_layout(SpriteAtlasLayout* layout) {
    layout->num_entries = 0;
    free(layout->entries);
    layout->entries = NULL;
}

Map dusk_load_map(char* name) {
    char ass_name[GBFS_ENTRY_SIZE];
    strcpy(ass_name, name);
    strcat(ass_name, ".bin");

    u32 map_data_sz = 0;
    const u16* map_data = gbfs_get_obj(gbfs_dat, ass_name, &map_data_sz);
    return map_load_from_rom(map_data);
}
