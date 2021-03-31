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

#define SPRITEFLAG_PRIORITY_SHIFT 6
#define SPRITEFLAG_PRIORITY_GET(n) ((n >> SPRITEFLAG_PRIORITY_SHIFT) & 0b11)
#define SPRITEFLAG_PRIORITY(n) ((n << SPRITEFLAG_PRIORITY_SHIFT) & 0b11)

/** sprite data */
typedef struct Sprite {
    s16 x, y;
    /** the number of tiles taken up by this sprite */
    u8 tile_sz;
    u16 tid;
    /** used for spritesheets/animation */
    u8 page;
    /** */
    u8 flags;
} Sprite;

/** animation metadata */
typedef struct Anim {
    /** which page in the sprite the animation starts */
    u8 start;
    /** number of pages in this animation */
    u8 len;
    /** (60/fps), or how many frames each page lasts */
    u8 rate;
} Anim;

/** define an animation, given a starting frame and frame count, and fps */
#define MAKE_ANIM_EX(st, le, fps)                                                                                      \
    (Anim) { .start = st, .len = le, .rate = (60 / fps) }
/** define an animation, given a starting frame and frame count, at 10 fps */
#define MAKE_ANIM(st, le) MAKE_ANIM_EX(st, le, 10)

/** memory block used to track sprites */
extern Sprite sprites[NUM_SPRITES];

/** initialize sprite and OAM memory */
void dusk_sprites_init();
/** upload a sprite atlas to tile and palette memory */
void dusk_sprites_upload_atlas(SpriteAtlas* atlas);
/** create a sprite and store it in index of sprite memory */
Sprite* dusk_sprites_make(int index, u8 width, u8 height, Sprite spr);
/** synchronize a sprite from the sprites array to OAM block (GPU memory) */
void dusk_sprites_sync(int index);
/** synchronize all sprites to OAM block, then upload OAM block to GPU */
void dusk_sprites_update();
/** play an animation on a sprite */
void dusk_sprites_anim_play(Sprite* spr, Anim* anim);
/** calculate the tid of a sprite given pos and sheet dimens */
u16 dusk_sprites_pos_to_tid(u16 x, u16 y, u16 sheet_width, u16 sheet_height);