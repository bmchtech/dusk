module dusk.sprites;

extern (C):

import dusk.sprites;
import dusk.sys;
import dusk.load;
import tonc;
import ldc.attributes;
import core.stdc.stdlib;
import core.stdc.stdio;
import core.stdc.string;

enum NUM_SPRITES = 128;
enum NUM_AFFINE_SPRITES = 32;

enum SPRITEFLAG_PRIORITY_SHIFT = 6;
// #define SPRITEFLAG_PRIORITY_GET(n) ((n >> SPRITEFLAG_PRIORITY_SHIFT) & 0b11)
// #define SPRITEFLAG_PRIORITY(n) ((n << SPRITEFLAG_PRIORITY_SHIFT) & 0b11)

enum DUSKSPRITE_FLAGS_VISIBLE = (0x1 << 0);

/** sprite data */
struct Sprite {
    /** the screen x and y of the sprite */
    s16 x, y;
    /** the number of tiles taken up by this sprite */
    u8 tile_sz;
    /** the raw tid of the top left corner tile of spritesheet frame 0 */
    u16 base_tid;
    /** the frame index of the spritesheet */
    u8 page;
    /** flags: 0/0/0/0/0/0/0/visible */
    u8 flags;

    /** initialize a sprite struct */
    static Sprite init(s16 x, s16 y, u8 page = 0, u8 flags = 0) {
        Sprite s;
        s.x = x;
        s.y = y;
        s.tile_sz = 1;
        s.base_tid = 0;
        s.page = page;
        s.flags = flags;
        return s;
    }
}

/** animation metadata */
struct Anim {
    /** which page in the sprite the animation starts */
    u8 start;
    /** number of pages in this animation */
    u8 len;
    /** (60/fps), or how many frames each page lasts */
    u8 rate;
}

/** background data */
struct Background {
    s16 x, y;
    int cbb, sbb;
}

/** define an animation, given a starting frame and frame count, and fps */
// #define MAKE_ANIM_EX(st, le, fps)                                                                                      \
//     (Anim) { .start = st, .len = le, .rate = (60 / fps) }
Anim make_anim_ex(u8 start, u8 len, u8 fps) {
    return Anim(start, len, 60 / fps);
}
/** define an animation, given a starting frame and frame count, at 10 fps */
// #define MAKE_ANIM(st, le) MAKE_ANIM_EX(st, le, 10)
Anim make_anim(u8 start, u8 len) {
    return make_anim_ex(start, len, 10);
}

// /** initialize sprite and OAM memory */
// void dusk_sprites_init();
// /** set mode options for dusk sprites */
// void dusk_sprites_configure(bool bpp8);
// /** upload a sprite atlas to tile and palette memory */
// void dusk_sprites_upload_atlas(SpriteAtlas* atlas);
// /** upload a sprite atlas section to tile and palette memory */
// void dusk_sprites_upload_atlas_section(SpriteAtlasLayout* layout, SpriteAtlas* atlas, SpriteAtlasEntry* entry, u16 pal_offset, u16 tile_offset);
// /** create a sprite and store it in index of sprite memory */
// Sprite* dusk_sprites_make(int index, u8 width, u8 height, Sprite spr);
// /** synchronize a sprite from the sprites array to OAM block (vram) */
// void dusk_sprites_sync(int index);
// /** synchronize all sprites to OAM block, then upload OAM block to vram */
// void dusk_sprites_update();
// /** play an animation on a sprite */
// void dusk_sprites_anim_play(Sprite* spr, Anim* anim);
// /** calculate the tid of a sprite given pos and sheet dimens */
// u16 dusk_sprites_pos_to_tid(u16 x, u16 y, u16 sheet_width, u16 sheet_height);
// /** upload image data to background tile vram */
// void dusk_background_upload_raw(GritImage* img, int cbb, int sbb);
// /** create a background and display it */
// void dusk_background_make(u8 bg_id, u16 size, Background bg);

// mixin(EWRAM_DATA!("OBJ_ATTR[NUM_SPRITES]", "obj_buffer"));
// static OBJ_ATTR[NUM_SPRITES] obj_buffer;
@(ldc.attributes.section(".ewram")) __gshared OBJ_ATTR[NUM_SPRITES] obj_buffer;
// OBJ_AFFINE* obj_aff_buffer = cast(OBJ_AFFINE*) obj_buffer;
__gshared OBJ_AFFINE* obj_aff_buffer() {
    return cast(OBJ_AFFINE*) obj_buffer;
}

/** memory block used to track sprites */
// mixin(EWRAM_DATA!("Sprite[NUM_SPRITES]", "sprites"));
// static Sprite[NUM_SPRITES] sprites;
@(ldc.attributes.section(".ewram")) __gshared Sprite[NUM_SPRITES] sprites;

/** when true, 8BPP, when false, 4BPP */
__gshared bool sprites_bpp8 = true;

void dusk_sprites_init() {
    // initialize object buffer
    oam_init(cast(OBJ_ATTR*) obj_buffer, NUM_SPRITES);
    memset(cast(void*) sprites, 0, (Sprite.sizeof) * NUM_SPRITES);

    // reset bpp to default
    sprites_bpp8 = true;

    // enable sprite display
    *REG_DISPCNT |= DCNT_OBJ | DCNT_OBJ_1D;
}

void dusk_sprites_configure(bool bpp8) {
    sprites_bpp8 = bpp8;
}

void dusk_sprites_upload_atlas(SpriteAtlas* atlas) {
    // 1. upload the atlas tile palette to palette memory
    // object/sprite palette, in 8bpp stores 256 colors
    memcpy(&pal_obj_bank[0], atlas.pal, atlas.pal_sz);
    // 2. upload the atlas tiles to tile memory
    // tile memory (can store 1024 tiles (32x32 tiles or 256x256px)
    // VRAM is charblocks 4 and 5, so &tile_mem[4][0] points to the first tile in object VRAM
    memcpy(&tile_mem[4][0], atlas.tiles, atlas.tile_sz);
}

void dusk_sprites_upload_atlas_section(SpriteAtlasLayout* layout, SpriteAtlas* atlas, SpriteAtlasEntry* entry,
    u16 pal_offset, u16 tile_offset) {

    // 1. upload the palette (palettes are 16-bit highcolor)
    memcpy(&pal_obj_bank[0][pal_offset], &atlas.pal[0], atlas.pal_sz);

    // pal_obj_bank[0][4] = CLR_YELLOW;
    // 2. upload the tiles
    int entry_firsttid =
        dusk_sprites_pos_to_tid(entry.x, entry.y, layout.width, layout.height); // tid of entry start in atlas
    int entry_tilecount = (entry.w) * (entry.h); // entry size in tiles
    int raw_firsttid = entry_firsttid * entry_tilecount * 2; // read offset
    // int raw_tilecount = entry_tilecount * 2;
    int raw_tileoffset = tile_offset * 2; // write offset

    // printf("ro: %d. wo: %d, n: %d\n", raw_firsttid, raw_tileoffset, raw_tilecount); // debug print tile r/w

    // memcpy(&tile_mem[4][raw_tileoffset], &atlas.tiles[raw_firsttid], entry_tilecount * 64);

    // 3. fix tiles to point at right palette
    // for (int i = 0; i < raw_tilecount; i += 2) {
    //     TILE8 new_tile;
    //     // TILE8 tile.data consists of 16 u32s (a total of 64 bytes)
    //     // new_tile.data[j] = atlas.tiles[(entry_firsttid + i) * 2];
    //     u32* rt = &atlas.tiles[raw_firsttid + i]; // read tile

    //     for (int j = 0; j < 16; j++) {
    //         new_tile.data[j] = rt[j];
    //         // new_tile.data[j] = 0x04040404;
    //     }
    //     // copy in tile
    //     memcpy(&tile_mem[4][raw_tileoffset + i], &new_tile, 64);
    // }

    // unroll malloc
    // memcpy(&tile_mem[4][raw_tileoffset], &atlas.tiles[raw_firsttid], 64);

    // reinterpret as byte pointers
    u8* loc_twrite = cast(u8*)&tile_mem[4][raw_tileoffset];
    u8* loc_tread = cast(u8*)&atlas.tiles[raw_firsttid];
    for (int i = 0; i < entry_tilecount; i += 1) {
        int c = i * 64;
        u8[64] tile; // create temp tile
        memcpy(&tile[0], loc_tread + c, 64); // read tile

        // fix tile
        for (int j = 0; j < 64; j++) {
            tile[j] = cast(ubyte)(tile[j] + pal_offset);
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
    spr.tile_sz = cast(ubyte)((width >> 3) * (height >> 3));

    // set main attributes
    // leave tile id (attr2) null, it will be set in sync
    u16 bpp_flag = (sprites_bpp8 == 1) ? ATTR0_8BPP : ATTR0_4BPP;
    obj_set_attr(&obj_buffer[index], ATTR0_REG | shape_attr | bpp_flag, size_attr, 0);

    // set default flags
    spr.flags |= (DUSKSPRITE_FLAGS_VISIBLE);

    // save sprite metadata
    sprites[index] = spr;

    // sync other attributes
    dusk_sprites_sync(index);

    // return pointer to this sprite
    return &sprites[index];
}

pragma(inline) void dusk_sprites_sync(int i) {
    Sprite* sprite = &sprites[i];
    OBJ_ATTR* obj = &obj_buffer[i];
    // position
    obj_set_pos(obj, sprite.x, sprite.y);

    // visibility
    if ((sprite.flags & DUSKSPRITE_FLAGS_VISIBLE) > 0) {
        obj_unhide(obj, ATTR0_REG);
    } else {
        obj_hide(obj);
    }

    // main attrs

    // logical base tid mode
    // obj_set_attr(obj, obj.attr0, obj.attr1, (sprite.tid + sprite.page) * sprite.tile_sz * 2);

    // raw base tid mode
    int bpp_mult = (sprites_bpp8 == 1) ? 2 : 1;
    u16 tid = cast(u16)((sprite.base_tid + (sprite.page * sprite.tile_sz)) * bpp_mult);
    obj_set_attr(obj, obj.attr0, obj.attr1, cast(u16)(ATTR2_ID!u16(tid) | ATTR2_PALBANK!u16(0)));
}

void dusk_sprites_update() {
    // sync all sprites
    for (int i = 0; i < NUM_SPRITES; i++) {
        dusk_sprites_sync(i);
    }

    // upload shadow oam to vram

    oam_copy(oam_mem, cast(OBJ_ATTR*) obj_buffer, NUM_SPRITES);
    // CpuFastSet(obj_buffer, oam_mem, NUM_SPRITES * 2);
}

void dusk_sprites_anim_play(Sprite* spr, Anim* anim) {
    // make sure page is within anim range
    if (spr.page < anim.start || spr.page >= (anim.start + anim.len)) {
        spr.page = anim.start; // reset to first
    }
    int ix = spr.page - anim.start;
    if (frame_count % anim.rate == 0) {
        ix = (ix + 1) % anim.len;
    }
    // set new frame
    spr.page = cast(u8)(anim.start + ix);
}

u16 dusk_sprites_pos_to_tid(u16 x, u16 y, u16 sheet_width, u16 sheet_height) {
    // calculate corner tile id
    // first get x and y in tile coords
    u16 xt = x;
    u16 yt = y;
    u16 imw = sheet_width >> 3; // compute with bitshift (sheet_width / 8)
    // u16 imh = sheet_height >> 3;
    u16 tid = cast(u16)((yt * imw) + xt);
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
    default:
        assert(0);
    }

    *REG_DISPCNT |= bg_flag;
}

vu16* dusk_get_background_register(u8 bg_id) {
    switch (bg_id) {
    case 0:
        return REG_BG0CNT;
    case 1:
        return REG_BG1CNT;
    case 2:
        return REG_BG2CNT;
    case 3:
        return REG_BG3CNT;
    default:
        return null;
    }
}

void dusk_background_upload_raw(GritImage* img, int cbb, int sbb, int pal_offset = 0) {
    // TODO: support selecting slot

    // 1. upload the atlas tile palette to bg palette memory
    // memcpy32(&pal_bg_bank[0], img.pal, img.pal_sz / 4);
    // // 2. upload the atlas tiles to bg tile memory (CBB)
    // memcpy32(&tile_mem[cbb][0], img.tiles, img.tile_sz / 4);
    // // 3. upload the map (SBB)
    // memcpy32(&se_mem[sbb][0], img.map, img.map_sz / 4);

    // fix tiles to point at new palette pos
    if (pal_offset > 0) {
        auto raw_tdata = cast(u8*) img.tiles;
        for (int i = 0; i < img.tile_sz; i += 64) {
            for (int j = 0; j < 64; j++) {
                raw_tdata[i + j] += pal_offset;
            }
        }
    }

    memcpy32(&pal_bg_bank[0][pal_offset], img.pal, img.pal_sz / 4);
    version (USE_DMA) {
        dma3_cpy(&tile_mem[cbb][0], img.tiles, img.tile_sz);
    } else {
        memcpy32(&tile_mem[cbb][0], img.tiles, img.tile_sz / 4);
    }
    memcpy32(&se_mem[sbb][0], img.map, img.map_sz / 4);
    // dma3_cpy(&pal_bg_bank[0], img.pal, img.pal_sz);
    // dma3_cpy(&se_mem[sbb][0], img.map, img.map_sz);
}

void dusk_background_make(u8 bg_id, u16 size, Background bg) {
    // set bg on screen enabled
    enable_bg(bg_id);
    // set control flags
    vu16* bg_reg = dusk_get_background_register(bg_id);

    // u16 bpp_flag = (sprites_bpp8 == 1) ? BG_8BPP : BG_4BPP;
    *bg_reg |= BG_CBB(bg.cbb) | BG_SBB(bg.sbb) | BG_8BPP | size;
}
