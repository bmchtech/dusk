#include "ds_spr.h"
#include <tonc.h>

OBJ_ATTR obj_buffer[128];
OBJ_AFFINE *obj_aff_buffer = (OBJ_AFFINE *)obj_buffer;

void dusk_sprites_init() {
    // initialize object buffer
    oam_init(obj_buffer, 128);

    // enable sprite display
    REG_DISPCNT |= DCNT_OBJ | DCNT_OBJ_1D;
}