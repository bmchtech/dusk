#include <stddef.h>
#include <string.h>
#include <tonc_types.h>
#include "ds_load.h"

const GBFS_FILE *gbfs_dat;

void dusk_load_init() {
    // locate the main GBFS archive
    gbfs_dat = find_first_gbfs_file(find_first_gbfs_file);
}

SpriteAtlas dusk_load_atlas(char* name) {
    SpriteAtlas atlas;
    char tiles_name[GBFS_ENTRY_SIZE];
    strcpy(tiles_name, name);
    strcat(tiles_name, ".img.bin");
    char pal_name[GBFS_ENTRY_SIZE];
    strcpy(pal_name, name);
    strcat(pal_name, ".pal.bin");

    atlas.tiles = gbfs_get_obj(gbfs_dat, tiles_name, &atlas.tile_sz);
    atlas.pal = gbfs_get_obj(gbfs_dat, pal_name, &atlas.pal_sz);

    return atlas;
}

Map dusk_load_map(char* name) {
    char ass_name[GBFS_ENTRY_SIZE];
    strcpy(ass_name, name);
    strcat(ass_name, ".bin");

    u32 map_data_sz = 0;
    const u16 *map_data = gbfs_get_obj(gbfs_dat, ass_name, &map_data_sz);
    return map_load_from_rom(map_data);
}