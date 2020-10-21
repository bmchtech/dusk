#include "gbamap.h"
#include "./background.h"

void map_init_registers() {
    for (u32 layerIndex = MAX_LAYERS; layerIndex-- > 0;) {
        REGISTER_BACKGROUND_CONTROL[layerIndex] = 0;
    }

    REGISTER_DISPLAY_CONTROL = 0;
}