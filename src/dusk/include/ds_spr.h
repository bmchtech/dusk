/*
    DUSK: SPRITES
*/

#pragma once

#include <tonc_oam.h>
#include <string.h>
#include "ds_load.h"

#define NUM_SPRITES 128
#define NUM_AFFINE_SPRITES 32
extern OBJ_ATTR obj_buffer[NUM_SPRITES];
extern OBJ_AFFINE *obj_aff_buffer;

typedef struct Sprite {
    u16 x, y;
    // u16 w, h;
    u16 tid;
    u8 page;
    u8 page_ct;
} Sprite;

extern Sprite sprites[NUM_SPRITES];

void dusk_sprites_init();
void dusk_sprites_upload_atlas(SpriteAtlas* atlas);
Sprite* dusk_sprites_make(int index, u8 width, u8 height, Sprite spr);
void dusk_sprites_sync(int index);
void dusk_sprites_update();