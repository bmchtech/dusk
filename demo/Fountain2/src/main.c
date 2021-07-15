#include "dusk.h"
#include "ds_sys.h"
#include "scenes.h"

int main() {
    dusk_init_all();

    dusk_scene_set(logo_scene);

    while (TRUE) {
        key_poll(); // update inputF
        dusk_scene_update();
    }
}