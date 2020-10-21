#include "dusk.h"
#include "scenes.h"

int main() {
    dusk_init();

    dusk_scene_set(fountain_scene);
    
    while (TRUE) {
        dusk_scene_update();
    }
}