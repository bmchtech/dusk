module dusk.load;

extern(C):

import core.stdc.stdio;
import core.stdc.stdlib;
import core.stdc.stddef;
import core.stdc.string;
import tonc.tonc_types;
import dusk.contrib.gbfs;
import dusk.contrib.gbamap;

// represents an atlas, with bitmaps for the image and palette
struct SpriteAtlas {
    u32* tiles;
    u32 tile_sz;
    u32* pal;
    u32 pal_sz;
}

struct GritImage {
    u32* tiles;
    u32 tile_sz;
    u32* pal;
    u32 pal_sz;
    u32* map;
    u32 map_sz;
}

enum ATLAS_ENTRY_LEN = 6;

/** represents a single entry in the sprite atlas.
 * note that names are limited to just 5 chars and a NULL terminator. */
struct SpriteAtlasEntry {
    char[ATLAS_ENTRY_LEN] name;
    // all of these are in tiles (8px)
    u8 x;
    u8 y;
    u8 w;
    u8 h;
}

/** represents the layout of items in an atlas (https://github.com/xdrie/crunch#binary-format).
 * we only use the first texture because the vram can only hold one */
struct SpriteAtlasLayout {
    u16 width;
    u16 height;
    u16 num_entries;
    SpriteAtlasEntry* entries;
}

/** initialize loader */
void dusk_load_init();

// - CONTENT LOADERS: pass filename without extension

// void* dusk_load_raw(char* name, u32* len);
// GritImage dusk_load_image(char* name);
// SpriteAtlas dusk_load_atlas(char* name);
// SpriteAtlasLayout dusk_load_atlas_layout(const char* name);
// SpriteAtlasEntry* dusk_load_atlas_entry(SpriteAtlasLayout* layout, const char* entry_name);
// void dusk_free_atlas_layout(SpriteAtlasLayout* layout);

// Map dusk_load_map(char* name);

__gshared static GBFS_FILE* gbfs_dat;

void dusk_load_init() {
    // locate the main GBFS archive
    gbfs_dat = cast(GBFS_FILE*) find_first_gbfs_file(&find_first_gbfs_file);
}

void* dusk_load_raw(char* name, u32* len) {
    return cast(void*) gbfs_get_obj(gbfs_dat, name, len);
}

GritImage dusk_load_image(char* name) {
    GritImage img;
    char[GBFS_ENTRY_SIZE] tiles_name;
    strcpy(cast(char*)tiles_name, name);
    strcat(cast(char*)tiles_name, cast(char*)".img.bin");
    char[GBFS_ENTRY_SIZE] pal_name;
    strcpy(cast(char*)pal_name, name);
    strcat(cast(char*)pal_name, cast(char*)".pal.bin");
    char[GBFS_ENTRY_SIZE] map_name;
    strcpy(cast(char*)map_name, name);
    strcat(cast(char*)map_name, cast(char*)".map.bin");

    img.tiles = cast(u32*)gbfs_get_obj(gbfs_dat, cast(char*)tiles_name, &img.tile_sz);
    img.pal = cast(u32*)gbfs_get_obj(gbfs_dat, cast(char*)pal_name, &img.pal_sz);
    img.map = cast(u32*)gbfs_get_obj(gbfs_dat, cast(char*)map_name, &img.map_sz);

    return img;
}

SpriteAtlas dusk_load_atlas(char* name) {
    char[GBFS_ENTRY_SIZE] atlas_name;
    strcpy(cast(char*)atlas_name, name);
    strcat(cast(char*)atlas_name, cast(char*)"_0");

    GritImage img = dusk_load_image(cast(char*)atlas_name);
    
    SpriteAtlas atlas;
    atlas.tiles = img.tiles;
    atlas.tile_sz = img.tile_sz;
    atlas.pal = img.pal;
    atlas.pal_sz = img.pal_sz;

    // 1. pal_sz is some weird gradient thing
    // we're going to detect when more than 4 in the row are the same
    // so then find the actual end of the palette

    // NOTE: ASSERT pal_sz >= 2

    u16* pal_raw = cast(u16*)atlas.pal; // 16-bit color
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
    char[GBFS_ENTRY_SIZE] file_name;
    strcpy(cast(char*)file_name, name);
    strcat(cast(char*)file_name, cast(char*)"_.sht.bin");

    u32 data_sz = 0;
    const u8* data = cast(u8*) gbfs_get_obj(gbfs_dat, cast(char*)file_name, &data_sz);

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
    layout.entries = cast(SpriteAtlasEntry*) malloc((SpriteAtlasEntry.sizeof) * layout.num_entries);
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
        layout.entries[i].x = cast(u8) x / 8;
        layout.entries[i].y = cast(u8) y / 8;
        layout.entries[i].w = cast(u8) w / 8;
        layout.entries[i].h = cast(u8) h / 8;
    }

    return layout;
}

SpriteAtlasEntry* dusk_load_atlas_entry(SpriteAtlasLayout* layout, const char* entry_name) {
    for (int i = 0; i < layout.num_entries; i++) {
        SpriteAtlasEntry* entry = &layout.entries[i];
        if (strncmp(cast(char*)entry_name, cast(char*)entry.name, ATLAS_ENTRY_LEN) == 0) {
            return entry;
        }
    }
    return null;
}

void dusk_free_atlas_layout(SpriteAtlasLayout* layout) {
    layout.num_entries = 0;
    free(layout.entries);
    layout.entries = null;
}

Map dusk_load_map(char* name) {
    char[GBFS_ENTRY_SIZE] ass_name;
    strcpy(cast(char*)ass_name, name);
    strcat(cast(char*)ass_name, cast(char*)".bin");

    u32 map_data_sz = 0;
    const u16* map_data = cast(u16*) gbfs_get_obj(gbfs_dat, cast(char*)ass_name, &map_data_sz);
    return map_load_from_rom(map_data);
}
