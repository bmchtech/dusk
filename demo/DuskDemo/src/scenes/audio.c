#include <string.h>
#include "dusk.h"
#include "tonc.h"
#include "maxmod.h"
#include "soundbank.h"

mm_sound_effect boom;

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

    // init text engine
    REG_DISPCNT |= DCNT_BG1;
    tte_init_chr4c(1, BG_CBB(0) | BG_SBB(31), 0, 0x0201, CLR_WHITE, NULL, NULL);
    tte_init_con();
    tte_printf("#{P:12,12}audio (bank: %d KB)", soundbank_len / 1000);
}

void audio_update() {
    dusk_frame(); // video frame    
    mmFrame(); // audio frame

    int a_press = key_is_down(KEY_A);

    if (a_press) {
        // play sfx
        mmEffectEx(&boom);
    }
}

void audio_end() {
    mmStop();
}

Scene audio_scene = {
    .start = audio_start,
    .update = audio_update,
    .end = audio_end,
};