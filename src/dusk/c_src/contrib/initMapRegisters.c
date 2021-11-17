#include "gbamap.h"
#include "./background.h"

void map_init_registers() {
    // turn off all backgrounds
    for (u32 layerIndex = MAX_LAYERS; layerIndex-- > 0;) {
        REGISTER_BACKGROUND_CONTROL[layerIndex] = 0;
    }

    // reset display control
    REGISTER_DISPLAY_CONTROL = 0;
}