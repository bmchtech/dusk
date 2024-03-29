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

#define DUSKSPRITE_FLAGS_VISIBLE (0x1 << 0)
/** sprite data */
typedef struct Sprite {
    s16 x, y;
    /** the number of tiles taken up by this sprite */
    u8 tile_sz;
    /** the raw tid of the top left corner tile of spritesheet frame 0 */
    u16 base_tid;
    /** the frame index of the spritesheet */
    u8 page;
    /** flags: 0/0/0/0/0/0/0/visible */
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

/** background data */
typedef struct Background {
    s16 x, y;
    int cbb, sbb;
} Background;

/** define an animation, given a starting frame and frame count, and fps */
#define MAKE_ANIM_EX(st, le, fps)                                                                                      \
    (Anim) { .start = st, .len = le, .rate = (60 / fps) }
/** define an animation, given a starting frame and frame count, at 10 fps */
#define MAKE_ANIM(st, le) MAKE_ANIM_EX(st, le, 10)

/** memory block used to track sprites */
extern Sprite sprites[NUM_SPRITES];

/** initialize sprite and OAM memory */
void dusk_sprites_init();
/** set mode options for dusk sprites */
void dusk_sprites_configure(BOOL bpp8);
/** upload a sprite atlas to tile and palette memory */
void dusk_sprites_upload_atlas(SpriteAtlas* atlas);
/** upload a sprite atlas section to tile and palette memory */
void dusk_sprites_upload_atlas_section(SpriteAtlasLayout* layout, SpriteAtlas* atlas, SpriteAtlasEntry* entry, u16 pal_offset, u16 tile_offset);
/** create a sprite and store it in index of sprite memory */
Sprite* dusk_sprites_make(int index, u8 width, u8 height, Sprite spr);
/** synchronize a sprite from the sprites array to OAM block (vram) */
void dusk_sprites_sync(int index);
/** synchronize all sprites to OAM block, then upload OAM block to vram */
void dusk_sprites_update();
/** play an animation on a sprite */
void dusk_sprites_anim_play(Sprite* spr, Anim* anim);
/** calculate the tid of a sprite given pos and sheet dimens */
u16 dusk_sprites_pos_to_tid(u16 x, u16 y, u16 sheet_width, u16 sheet_height);
/** upload image data to background tile vram */
void dusk_background_upload_raw(GritImage* img, int cbb, int sbb);
/** create a background and display it */
void dusk_background_make(u8 bg_id, u16 size, Background bg);