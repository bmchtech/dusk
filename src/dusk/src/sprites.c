#include "ds_spr.h"
#include "ds_sys.h"
#include <tonc.h>

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE* obj_aff_buffer = (OBJ_AFFINE*)obj_buffer;

Sprite sprites[NUM_SPRITES];

#define BPP 8

void dusk_sprites_init() {
    // initialize object buffer
    oam_init(obj_buffer, 128);
    memset(sprites, 0, sizeof(Sprite) * NUM_SPRITES);

    // enable sprite display
    REG_DISPCNT |= DCNT_OBJ | DCNT_OBJ_1D;
}

void dusk_sprites_upload_atlas(SpriteAtlas* atlas) {
    // tile memory (can store 1024 tiles, or 32x32b, or 256x256p)
    memcpy(&tile_mem[4][0], atlas->tiles, atlas->tile_sz);
    // object/sprite palette, in 8bpp stores 256 colors
    memcpy(&pal_obj_bank[0], atlas->pal, atlas->pal_sz);
}

Sprite* dusk_sprites_make(int index, u8 width, u8 height, Sprite spr) {
    // spr.w = width;
    // spr.h = height;

    // automatically figure out size params
    u16 shape = 0;
    if (height > width) {
        shape = ATTR0_TALL;
    } else if (width > height) {
        shape = ATTR0_WIDE;
    } else if (width == height) {
        shape = ATTR0_SQUARE;
    }
    u16 size = 0;
    if (shape == ATTR0_TALL) {
        if (width == 8 && height == 16)
            size = ATTR1_SIZE_8x16;
        if (width == 8 && height == 32)
            size = ATTR1_SIZE_8x32;
        if (width == 16 && height == 32)
            size = ATTR1_SIZE_16x32;
        if (width == 16 && height == 64)
            size = ATTR1_SIZE_32x64;
    }
    if (shape == ATTR0_WIDE) {
        if (width == 16 && height == 8)
            size = ATTR1_SIZE_16x8;
        if (width == 32 && height == 8)
            size = ATTR1_SIZE_32x8;
        if (width == 32 && height == 16)
            size = ATTR1_SIZE_32x16;
        if (width == 64 && height == 32)
            size = ATTR1_SIZE_64x32;
    }
    if (shape == ATTR0_SQUARE) {
        if (width == 8 && height == 8)
            size = ATTR1_SIZE_8x8;
        if (width == 16 && height == 16)
            size = ATTR1_SIZE_16x16;
        if (width == 32 && height == 32)
            size = ATTR1_SIZE_32x32;
        if (width == 64 && height == 64)
            size = ATTR1_SIZE_64x64;
    }

    // set main attributes
    obj_set_attr(&obj_buffer[index], shape | ATTR0_8BPP, size, (spr.tid + spr.page) * BPP);

    // save sprite metadata
    sprites[index] = spr;

    // sync other attributes
    dusk_sprites_sync(index);

    // return pointer to this sprite
    return &sprites[index];
}

inline void dusk_sprites_sync(int i) {
    OBJ_ATTR* obj = &obj_buffer[i];
    // position
    obj_set_pos(obj, sprites[i].x, sprites[i].y);
    // main attrs
    obj_set_attr(obj, obj->attr0, obj->attr1, (sprites[i].tid + sprites[i].page) * BPP);
}

void dusk_sprites_update() {
    // sync all sprites
    for (int i = 0; i < NUM_SPRITES; i++) {
        dusk_sprites_sync(i);
    }

    // upload to gpu memory
    oam_copy(oam_mem, obj_buffer, NUM_SPRITES);
}

void dusk_sprites_anim_play(Sprite* spr, Anim* anim) {
    // make sure page is within anim range
    if (spr->page < anim->start || spr->page >= (anim->start + anim->len)) {
        spr->page = anim->start; // reset to first
    }
    int ix = spr->page - anim->start;
    if (frame_count % 6 == 0) {
        ix = (ix + 1) % anim->len;
    }
    // set new frame
    spr->page = anim->start + ix;
}