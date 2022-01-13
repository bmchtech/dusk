#ifndef MAP_H
#define MAP_H

typedef struct {
    unsigned short x, y;
} __attribute__((aligned(4))) ObjectPoint;

typedef struct {
    unsigned int id;
    ObjectPoint position;
    const char* name;
    const char* type;
} MapObject;

MapObject map_load_object(const unsigned short* object_data, unsigned short* index);
unsigned int map_load_object_id(const unsigned short* idData, unsigned short* index);
ObjectPoint map_load_object_position(const unsigned short* positionData, unsigned short* index);
const char* map_load_string(const unsigned short* stringData, unsigned short* index);
unsigned short map_pop_value(const unsigned short* data, unsigned short* index);

void map_shift_objects(MapObject* objects, ObjectPoint shift, unsigned int count);

typedef struct Map {
    unsigned short sizeFlag, paletteLength, tileSetLength, terrainMapLength, numObjects, numLayers, tileMapLength;

    const unsigned short *palette, *tileSet, *terrainMap;

    MapObject objects[256];

    const unsigned short* tileMapLayers[3];
} Map;

typedef struct BackgroundPoint {
    short x, y;
} __attribute__((aligned(4))) BackgroundPoint;

#define MAX_LAYERS 3
#define ENTRIES_IN_SCREEN_BLOCK 512
#define NUM_SCREEN_BLOCKS 31

Map map_load_from_rom(const unsigned short* map_data);

void map_init_registers();
void map_set_onscreen(Map map);
void map_shift(Map map, BackgroundPoint offset);
void map_shift_layer(unsigned short layer, BackgroundPoint offset);

#endif
