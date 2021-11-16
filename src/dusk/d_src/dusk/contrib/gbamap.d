module dusk.contrib.gbamap;

extern (C):

struct Map {
    ushort sizeFlag,
        paletteLength,
        tileSetLength,
        terrainMapLength,
        numObjects,
        numLayers,
        tileMapLength;

    const ushort *palette;
    const ushort *tileSet;
    const ushort *terrainMap;

    MapObject[256] objects;

    const ushort[3] *tileMapLayers;
}

align(2) struct BackgroundPoint {
    align (2):
        short x, y;
}

enum MAX_LAYERS = 3;
enum ENTRIES_IN_SCREEN_BLOCK = 512;
enum NUM_SCREEN_BLOCKS = 31;

Map map_load_from_rom(const ushort *map_data);

void map_init_registers();
void map_set_onscreen(Map map);
void map_shift(Map map, BackgroundPoint offset);
void map_shift_layer(ushort layer, BackgroundPoint offset);

align(4) struct ObjectPoint {
    align(4):
    ushort x, y;
}

struct MapObject
{
    uint id;
    ObjectPoint position;
    const char* name;
    const char* type;
}

MapObject map_load_object(const ushort* object_data, ushort* index);
uint map_load_object_id(const ushort* idData, ushort* index);
ObjectPoint map_load_object_position(const ushort* positionData, ushort* index);
char* map_load_string(const ushort* stringData, ushort* index);
ushort map_pop_value(const ushort* data, ushort* index);

void map_shift_objects(MapObject* objects, ObjectPoint shift, uint count);