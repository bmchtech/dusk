#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <tonc_types.h>
#include "ds_load.h"
#include <stdlib.h>

const GBFS_FILE* gbfs_dat;

void dusk_load_init() {
    // locate the main GBFS archive
    gbfs_dat = find_first_gbfs_file(find_first_gbfs_file);
}

SpriteAtlas dusk_load_atlas(char* name) {
    SpriteAtlas atlas;
    char tiles_name[GBFS_ENTRY_SIZE];
    strcpy(tiles_name, name);
    strcat(tiles_name, "_0.img.bin");
    char pal_name[GBFS_ENTRY_SIZE];
    strcpy(pal_name, name);
    strcat(pal_name, "_0.pal.bin");

    atlas.tiles = gbfs_get_obj(gbfs_dat, tiles_name, &atlas.tile_sz);
    atlas.pal = gbfs_get_obj(gbfs_dat, pal_name, &atlas.pal_sz);

    return atlas;
}

void seek_until_null(u8* data, int* pos) {
    char last = data[*pos];
    while (last != 0) {
        *pos = *pos + 1; // advance pointer
        last = data[*pos];
    }
}

SpriteAtlasLayout dusk_load_atlas_layout(char* name) {
    SpriteAtlasLayout layout;
    char file_name[GBFS_ENTRY_SIZE];
    strcpy(file_name, name);
    strcat(file_name, "_.sht.bin");

    u32 data_sz;
    u8* data = gbfs_get_obj(gbfs_dat, file_name, &data_sz);

    int pos = 0;

    // get the number of textures
    int num_tex = data[pos] | (data[pos + 1] << 8);
    assert(num_tex == 1); // there should only be a single texture
    pos += 2;
    // eat the texture name
    seek_until_null(data, &pos);
    pos++;

    layout.num_entries = data[pos] | (data[pos + 1] << 8);
    layout.entries = malloc(sizeof(SpriteAtlasEntry) * layout.num_entries);
    pos += 2;

    // now read blocks
    for (int i = 0; i < layout.num_entries; i++)
    {
        // save the entry name
        char last = data[pos];
        int name_len = 0;
        while (last != 0)
        {
            layout.entries[i].name[name_len] = last;
            name_len++;
            last = data[pos + name_len];
        }
        layout.entries[i].name[name_len] = '\0'; // null terminator
        pos += name_len + 1;                     // skip the name

        // x,y,w,h
        layout.entries[i].x = data[pos] | (data[pos + 1] << 8);
        pos += 2;
        layout.entries[i].y = data[pos] | (data[pos + 1] << 8);
        pos += 2;
        layout.entries[i].w = data[pos] | (data[pos + 1] << 8);
        pos += 2;
        layout.entries[i].h = data[pos] | (data[pos + 1] << 8);
        pos += 2;
    }

    return layout;
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