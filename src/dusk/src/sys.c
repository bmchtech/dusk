#include <tonc.h>
#include "ds_sys.h"
#include "ds_load.h"

void dusk_init() {
    // 1. initialize system graphics
    // mode 0
    REG_DISPCNT = DCNT_MODE0;

    // initialize loader
    dusk_load_init();
}