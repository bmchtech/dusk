#include "dusk.h"
#include "scenes.h"

#define NUM_DEMOS 2

int main() {
    dusk_init();

    Scene demos[NUM_DEMOS] = {
        fountain_scene,
        fountain_scene
    };
    int demo_ix = 0;

    dusk_scene_set(demos[demo_ix]);

    while (TRUE) {
        key_poll();
        if (key_was_down(KEY_START)) {
            demo_ix = (++demo_ix) % NUM_DEMOS;
            dusk_scene_set(demos[demo_ix]);
        }

        dusk_scene_update();
    }
}