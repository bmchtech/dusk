#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "maxmod.h"
#include "soundbank.h"

mm_sound_effect boom;
Sprite* vizspr;
Anim vz_anim1;

void audio_start() {
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
    mmStart(MOD_FLATOUTLIES, MM_PLAY_LOOP);

    // define sfx
    boom.handle = 0;
    boom.id = SFX_BOOM;
    boom.rate = (int)(1.0f * (1 << 10));
    boom.volume = 255;
    boom.panning = 255;

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
    tte_init_chr4c(1, BG_CBB(0) | BG_SBB(31), 0, 0x0201, CLR_GRAY, NULL, NULL);
    tte_init_con();
    // tte_printf("#{P:12,12}audio (bank: %d KB)", soundbank_len / 1000); // this line is problematic on TempGBA
    tte_printf("#{P:12,12} § music demo");
    tte_printf("#{P:200,140}next >");
}

void audio_update() {
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
}

void audio_end() {
    // mmStop();
}

Scene audio_scene = {
    .start = audio_start,
    .update = audio_update,
    .end = audio_end,
};