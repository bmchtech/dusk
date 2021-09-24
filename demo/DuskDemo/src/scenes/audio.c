#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "maxmod.h"
#include "soundbank.h"

mm_sound_effect boom;
Sprite* vizspr;
Anim vz_anim1;

void audio_start() {
    REG_WIN0H = 0xBEA0;

    dusk_init_graphics_mode0();

    // irq setup for maxmod dma
    irq_init(NULL);
    irq_add(II_VBLANK, mmVBlank);
    irq_enable(II_VBLANK);

    // load soundbank
    u32 soundbank_len;
    const u32* soundbank_bin = gbfs_get_obj(gbfs_dat, "soundbank.bin", &soundbank_len);

    // initialise maxmod with soundbank and 8 channels
    mmInitDefault((mm_addr)soundbank_bin, 8);

    // play module
    mmStart(MOD_RUNU5, MM_PLAY_LOOP);

    // define sfx
    boom.handle = 0;
    boom.id = SFX_BOOM;
    boom.rate = (int)(1.0f * (1 << 10));
    boom.volume = 255;
    boom.panning = 255;

    pal_bg_mem[0] = 0x0C02; // background color

    // load sprite atlas
    dusk_sprites_init();
    SpriteAtlas atlas = dusk_load_atlas("atl_mus");
    dusk_sprites_upload_atlas(&atlas);

    vizspr = dusk_sprites_make(0, 64, 64,
                               (Sprite){
                                   .x = SCREEN_WIDTH / 2 - 32,
                                   .y = SCREEN_HEIGHT / 2 - 32,
                                   .tid = 0,
                               });
    vz_anim1 = MAKE_ANIM(0, 16);

    // init text engine
    REG_DISPCNT |= DCNT_BG1;

    REG_WIN0H = 0xBEA1;
    tte_init_chr4c_default(1, BG_CBB(0) | BG_SBB(31));
    REG_WIN0H = 0xBEA2;
    tte_init_con();
    REG_WIN0H = 0xBEA3;
    
    tte_printf("music");
    REG_WIN0H = 0xBEA4;
}

void audio_update() {
    REG_WIN0H = 0xCAC0;
    dusk_frame(); // video frame
    mmFrame();    // audio frame

    dusk_sprites_anim_play(vizspr, &vz_anim1);

    int a_press = key_is_down(KEY_A);

    if (a_press) {
        // play sfx
        mmEffectEx(&boom);
    }

    // update sprites
    dusk_sprites_update();
    REG_WIN0H = 0xCAC1;
}

void audio_end() {
    mmStop();
}

Scene audio_scene = {
    .start = audio_start,
    .update = audio_update,
    .end = audio_end,
};