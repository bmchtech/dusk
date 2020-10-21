/*
    DUSK: SPRITES
*/

#pragma once

#include <tonc_oam.h>
#include <string.h>
#include "ds_load.h"

extern OBJ_ATTR obj_buffer[128];
extern OBJ_AFFINE *obj_aff_buffer;

void dusk_sprites_init();
void dusk_sprites_upload_atlas(SpriteAtlas* atlas);