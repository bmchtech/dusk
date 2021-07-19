#include <string.h>
#include "dusk.h"
#include "tonc.h"

Sprite* me_spr1;
Sprite* me_spr2;
Anim me_anim1;

void merge_start() {
    dusk_init_graphics_mode0();

    pal_bg_mem[0] = 0x0C02; // background color

    REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, BG_CBB(0) | BG_SBB(31), 0, 0x0201, CLR_GRAY, NULL, NULL);
    tte_init_con();

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas chrs_atlas = dusk_load_atlas("atl_chars");
    SpriteAtlasLayout chrs_atlas_layout = dusk_load_atlas_layout("atl_chars");

    SpriteAtlas logo_atlas = dusk_load_atlas("atl_logo");
    SpriteAtlasLayout logo_atlas_layout = dusk_load_atlas_layout("atl_logo");

    dusk_sprites_upload_atlas_section(&chrs_atlas_layout, &chrs_atlas, &chrs_atlas_layout.entries[2], 0, 16, 0);
    dusk_sprites_upload_atlas_section(&logo_atlas_layout, &logo_atlas, &logo_atlas_layout.entries[0], 16, 16, 16);
    // dusk_sprites_upload_atlas_section(&logo_atlas_layout, &logo_atlas, &logo_atlas_layout.entries[0], 0, 0);

    me_spr1 = dusk_sprites_make(0, 16, 16, (Sprite){.x = 40, .y = 40, .tid = 0, .flags = SPRITEFLAG_PRIORITY(3)});
    me_spr2 = dusk_sprites_make(1, 64, 64, (Sprite){.x = 80, .y = 80, .tid = 16});
    me_anim1 = MAKE_ANIM(0, 4);

    SpriteAtlas print_atlas = logo_atlas;
    SpriteAtlasLayout print_atlas_layout = logo_atlas_layout;

    printf("ne: %d ", print_atlas_layout.num_entries);
    SpriteAtlasEntry* th_ent = &print_atlas_layout.entries[0];
    printf("ent: %s, %d, %d, %d, %d\n", th_ent->name, th_ent->x, th_ent->y, th_ent->w, th_ent->h);
    printf("atl: %d, %d ", print_atlas.pal_sz, print_atlas.tile_sz);
    
    // tte_printf("#{P:12,12}merge");
    // tte_printf("#{P:200,140}next >");
}

void merge_update() {
    dusk_frame();

    dusk_sprites_anim_play(me_spr1, &me_anim1);

    // update sprites
    dusk_sprites_update();
}

void merge_end() {}

Scene merge_scene = {
    .start = merge_start,
    .update = merge_update,
    .end = merge_end,
};