#include "gbamap.h"
#include <string.h>
#include "./types.h"

Map map_load_from_rom(const u16 *map_data) {
    Map map;
    u16 index = 0;

    map.sizeFlag = map_data[index++];

    map.paletteLength = map_data[index++];
    map.palette = &map_data[index];
    index += map.paletteLength + 1;

    map.tileSetLength = map_data[index++];
    map.tileSet = &map_data[index];
    index += map.tileSetLength + 1;

    map.terrainMapLength = map_data[index++];
    map.terrainMap = &map_data[index];
    index += map.terrainMapLength;

    map.numLayers = map_data[index++];
    map.numLayers = map.numLayers > MAX_LAYERS ? MAX_LAYERS : map.numLayers;
    map.tileMapLength = map_data[index++];
    for (u32 layerIndex = 0; layerIndex < map.numLayers; ++layerIndex)
    {
        map.tileMapLayers[layerIndex] = &map_data[index];
        index += map.tileMapLength;
    }

    u16 lengthObjectData = map_data[index++];
    u32 endObjectData = index + lengthObjectData;
    u32 objectCount = 0;
    while (index != endObjectData) {
        MapObject object = map_load_object(map_data, &index);
        map.objects[objectCount] = object;
        objectCount++;
    }
    map.numObjects = objectCount;

    return map;
}
