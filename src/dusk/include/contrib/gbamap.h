#ifndef MAP_H
#define MAP_H

#include "gbamap_object.h"

typedef struct Map {
    unsigned short sizeFlag,
        paletteLength,
        tileSetLength,
        terrainMapLength,
        numObjects,
        numLayers,
        tileMapLength;

    const unsigned short *palette,
              *tileSet,
              *terrainMap;

    MapObject objects[256];

    const unsigned short *tileMapLayers[3];
} Map;

typedef struct BackgroundPoint {
    short x, y;
} __attribute__((aligned(4))) BackgroundPoint;

#define MAX_LAYERS 3
#define ENTRIES_IN_SCREEN_BLOCK 512
#define NUM_SCREEN_BLOCKS 31

Map map_load_from_rom(const unsigned short *map_data);

void map_init_registers();
void map_set_onscreen(Map map);
void map_shift(Map map, BackgroundPoint offset);
void map_shift_layer(unsigned short layer, BackgroundPoint offset);

#endif
