#include "gbamap.h"
#include "./background.h"

void map_shift(Map map, BackgroundPoint offset)
{
    for (u16 layer = 0; layer < map.numLayers; layer++)
        map_shift_layer(layer, offset);
}
