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
extern OBJ_AFFINE* obj_aff_buffer;

typedef struct Sprite {
    s16 x, y;
    u16 tile_sz; // the number of tiles taken up by this sprite
    u16 tid;
    u8 page; // used for spritesheets
} Sprite;

typedef struct Anim {
    u8 start; // which page in the sprite the animation starts
    u8 len;   // number of pages in this animation
    u8 rate;  // (60/fps), or how many frames each page lasts
} Anim;
#define MAKE_ANIM_EX(st, le, fps)                                                                                      \
    (Anim) { .start = st, .len = le, .rate = (60 / fps) }
#define MAKE_ANIM(st, le) MAKE_ANIM_EX(st, le, 10) // default to 10fps

extern Sprite sprites[NUM_SPRITES];

void dusk_sprites_init();                           // initialize sprite and OAM memory
void dusk_sprites_upload_atlas(SpriteAtlas* atlas); // upload a sprite atlas to the sprite tiles and palettes
Sprite* dusk_sprites_make(int index, u8 width, u8 height, Sprite spr); // create a sprite at index
void dusk_sprites_sync(int index);                                     // synchronize sprite objects to OAM format
void dusk_sprites_update();                                            // upload object data to OAM
void dusk_sprites_anim_play(Sprite* spr, Anim* anim);                  // play frame animation on sprite
u16 dusk_sprites_pos_to_tid(u16 x, u16 y, u16 sheet_width, u16 sheet_height); // get tid from spritesheet position