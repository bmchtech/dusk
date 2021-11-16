module dusk.sprites;

#include "ds_spr.h"
#include "ds_sys.h"
#include <tonc.h>
#include "stdio.h"

EWRAM_DATA OBJ_ATTR obj_buffer[NUM_SPRITES];
OBJ_AFFINE* obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

EWRAM_DATA Sprite sprites[NUM_SPRITES];

/** when true, 8BPP, when false, 4BPP */
BOOL sprites_bpp8 = TRUE;

void dusk_sprites_init() {
    // initialize object buffer
    oam_init(obj_buffer, NUM_SPRITES);
    memset(sprites, 0, sizeof(Sprite) * NUM_SPRITES);

    // reset bpp to default
    sprites_bpp8 = TRUE;

    // enable sprite display
    REG_DISPCNT |= DCNT_OBJ | DCNT_OBJ_1D;
}

void dusk_sprites_configure(BOOL bpp8) { sprites_bpp8 = bpp8; }

void dusk_sprites_upload_atlas(SpriteAtlas* atlas) {
    // 1. upload the atlas tile palette to palette memory
    // object/sprite palette, in 8bpp stores 256 colors
    memcpy(&pal_obj_bank[0], atlas->pal, atlas->pal_sz);
    // 2. upload the atlas tiles to tile memory
    // tile memory (can store 1024 tiles (32x32 tiles or 256x256px)
    // VRAM is charblocks 4 and 5, so &tile_mem[4][0] points to the first tile in object VRAM
    memcpy(&tile_mem[4][0], atlas->tiles, atlas->tile_sz);
}

void dusk_sprites_upload_atlas_section(SpriteAtlasLayout* layout, SpriteAtlas* atlas, SpriteAtlasEntry* entry,
                                       u16 pal_offset, u16 tile_offset) {

    // 1. upload the palette (palettes are 16-bit highcolor)
    memcpy(&pal_obj_bank[0][pal_offset], &atlas->pal[0], atlas->pal_sz);

    // pal_obj_bank[0][4] = CLR_YELLOW;
    // 2. upload the tiles
    int entry_firsttid =
        dusk_sprites_pos_to_tid(entry->x, entry->y, layout->width, layout->height); // tid of entry start in atlas
    int entry_tilecount = (entry->w) * (entry->h);                                  // entry size in tiles
    int raw_firsttid = entry_firsttid * entry_tilecount * 2;                        // read offset
    // int raw_tilecount = entry_tilecount * 2;
    int raw_tileoffset = tile_offset * 2; // write offset

    // printf("ro: %d. wo: %d, n: %d\n", raw_firsttid, raw_tileoffset, raw_tilecount); // debug print tile r/w

    // memcpy(&tile_mem[4][raw_tileoffset], &atlas->tiles[raw_firsttid], entry_tilecount * 64);

    // 3. fix tiles to point at right palette
    // for (int i = 0; i < raw_tilecount; i += 2) {
    //     TILE8 new_tile;
    //     // TILE8 tile.data consists of 16 u32s (a total of 64 bytes)
    //     // new_tile.data[j] = atlas->tiles[(entry_firsttid + i) * 2];
    //     u32* rt = &atlas->tiles[raw_firsttid + i]; // read tile

    //     for (int j = 0; j < 16; j++) {
    //         new_tile.data[j] = rt[j];
    //         // new_tile.data[j] = 0x04040404;
    //     }
    //     // copy in tile
    //     memcpy(&tile_mem[4][raw_tileoffset + i], &new_tile, 64);
    // }

    // unroll malloc
    // memcpy(&tile_mem[4][raw_tileoffset], &atlas->tiles[raw_firsttid], 64);

    // reinterpret as byte pointers
    u8* loc_twrite = (u8*)&tile_mem[4][raw_tileoffset];
    u8* loc_tread = (u8*)&atlas->tiles[raw_firsttid];
    for (int i = 0; i < entry_tilecount; i += 1) {
        int c = i * 64;
        u8 tile[64];                         // create temp tile
        memcpy(&tile[0], loc_tread + c, 64); // read tile

        // fix tile
        for (int j = 0; j < 64; j++) {
            tile[j] = tile[j] + pal_offset;
        }

        memcpy(loc_twrite + c, &tile[0], 64); // write tile
    }
}

Sprite* dusk_sprites_make(int index, u8 width, u8 height, Sprite spr) {
    // automatically figure out sprite size/shape attributes
    u16 shape_attr = 0;
    if (height > width) {
        shape_attr = ATTR0_TALL;
    } else if (width > height) {
        shape_attr = ATTR0_WIDE;
    } else if (width == height) {
        shape_attr = ATTR0_SQUARE;
    }
    u16 size_attr = 0;
    if (shape_attr == ATTR0_TALL) {
        if (width == 8 && height == 16)
            size_attr = ATTR1_SIZE_8x16;
        if (width == 8 && height == 32)
            size_attr = ATTR1_SIZE_8x32;
        if (width == 16 && height == 32)
            size_attr = ATTR1_SIZE_16x32;
        if (width == 16 && height == 64)
            size_attr = ATTR1_SIZE_32x64;
    }
    if (shape_attr == ATTR0_WIDE) {
        if (width == 16 && height == 8)
            size_attr = ATTR1_SIZE_16x8;
        if (width == 32 && height == 8)
            size_attr = ATTR1_SIZE_32x8;
        if (width == 32 && height == 16)
            size_attr = ATTR1_SIZE_32x16;
        if (width == 64 && height == 32)
            size_attr = ATTR1_SIZE_64x32;
    }
    if (shape_attr == ATTR0_SQUARE) {
        if (width == 8 && height == 8)
            size_attr = ATTR1_SIZE_8x8;
        if (width == 16 && height == 16)
            size_attr = ATTR1_SIZE_16x16;
        if (width == 32 && height == 32)
            size_attr = ATTR1_SIZE_32x32;
        if (width == 64 && height == 64)
            size_attr = ATTR1_SIZE_64x64;
    }

    // calculate the number of tiles used by one frame of this sprite (divide width and height by 8)
    spr.tile_sz = (width >> 3) * (height >> 3);

    // set main attributes
    // leave tile id (attr2) null, it will be set in sync
    u16 bpp_flag = (sprites_bpp8 == 1) ? ATTR0_8BPP : ATTR0_4BPP;
    obj_set_attr(&obj_buffer[index], ATTR0_REG | shape_attr | bpp_flag, size_attr, 0);

    // set default flags
    spr.flags = (DUSKSPRITE_FLAGS_VISIBLE);

    // save sprite metadata
    sprites[index] = spr;

    // sync other attributes
    dusk_sprites_sync(index);

    // return pointer to this sprite
    return &sprites[index];
}

inline void dusk_sprites_sync(int i) {
    Sprite* sprite = &sprites[i];
    OBJ_ATTR* obj = &obj_buffer[i];
    // position
    obj_set_pos(obj, sprite->x, sprite->y);

    // visibility
    if ((sprite->flags & DUSKSPRITE_FLAGS_VISIBLE) > 0) {
        obj_unhide(obj, ATTR0_REG);
    } else {
        obj_hide(obj);
    }

    // main attrs

    // logical base tid mode
    // obj_set_attr(obj, obj->attr0, obj->attr1, (sprite->tid + sprite->page) * sprite->tile_sz * 2);

    // raw base tid mode
    int bpp_mult = (sprites_bpp8 == 1) ? 2 : 1;
    u16 tid = (sprite->base_tid + (sprite->page * sprite->tile_sz)) * bpp_mult;
    obj_set_attr(obj, obj->attr0, obj->attr1, ATTR2_ID(tid) | ATTR2_PALBANK(0));
}

void dusk_sprites_update() {
    // sync all sprites
    for (int i = 0; i < NUM_SPRITES; i++) {
        dusk_sprites_sync(i);
    }

    // upload shadow oam to vram

    oam_copy(oam_mem, obj_buffer, NUM_SPRITES);
    // CpuFastSet(obj_buffer, oam_mem, NUM_SPRITES * 2);
}

void dusk_sprites_anim_play(Sprite* spr, Anim* anim) {
    // make sure page is within anim range
    if (spr->page < anim->start || spr->page >= (anim->start + anim->len)) {
        spr->page = anim->start; // reset to first
    }
    int ix = spr->page - anim->start;
    if (frame_count % anim->rate == 0) {
        ix = (ix + 1) % anim->len;
    }
    // set new frame
    spr->page = anim->start + ix;
}

u16 dusk_sprites_pos_to_tid(u16 x, u16 y, u16 sheet_width, u16 sheet_height) {
    // calculate corner tile id
    // first get x and y in tile coords
    u16 xt = x;
    u16 yt = y;
    u16 imw = sheet_width >> 3; // compute with bitshift (sheet_width / 8)
    // u16 imh = sheet_height >> 3;
    u16 tid = (yt * imw) + xt;
    return tid;
}

void enable_bg(u8 bg_id) {
    int bg_flag = 0;
    switch (bg_id) {
    case 0:
        bg_flag = DCNT_BG0;
        break;
    case 1:
        bg_flag = DCNT_BG1;
        break;
    case 2:
        bg_flag = DCNT_BG2;
        break;
    case 3:
        bg_flag = DCNT_BG3;
        break;
    }

    REG_DISPCNT |= bg_flag;
}

vu16* dusk_get_background_register(u8 bg_id) {
    switch (bg_id) {
    case 0:
        return &REG_BG0CNT;
    case 1:
        return &REG_BG1CNT;
    case 2:
        return &REG_BG2CNT;
    case 3:
        return &REG_BG3CNT;
    default:
        return NULL;
    }
}

void dusk_background_upload_raw(GritImage* img, int cbb, int sbb) {
    // TODO: support selecting slot

    // 1. upload the atlas tile palette to bg palette memory
    memcpy(&pal_bg_bank[0], img->pal, img->pal_sz);
    // 2. upload the atlas tiles to bg tile memory (CBB)
    memcpy(&tile_mem[cbb][0], img->tiles, img->tile_sz);
    // 3. upload the map (SBB)
    memcpy(&se_mem[sbb][0], img->map, img->map_sz);
}

void dusk_background_make(u8 bg_id, u16 size, Background bg) {
    // set bg on screen enabled
    enable_bg(bg_id);
    // set control flags
    vu16* bg_reg = dusk_get_background_register(bg_id);

    // u16 bpp_flag = (sprites_bpp8 == 1) ? BG_8BPP : BG_4BPP;
    *bg_reg |= BG_CBB(bg.cbb) | BG_SBB(bg.sbb) | BG_8BPP | size;
}