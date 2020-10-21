#include "./background.h"
#include "gbamap.h"

void map_shift_layer(u16 layer, BackgroundPoint offset)
{
    REGISTER_BACKGROUND_OFFSET[layer] = offset;
}
