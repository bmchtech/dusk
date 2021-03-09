#include "dusk.h"
#include "ds_sys.h"

extern Scene fountain_scene;

int main() {
    dusk_init_all();

    dusk_scene_set(fountain_scene);
}