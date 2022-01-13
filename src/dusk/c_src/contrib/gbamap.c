#include <string.h>
#include "gbamap.h"

typedef unsigned char u8, byte;
typedef unsigned short u16, hword;
typedef unsigned int u32, word;
typedef unsigned long long u64;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;

typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;

#define INLINE static inline
#define ALIGN(n) __attribute__((aligned(n)))

#define TRUE 1
#define FALSE 0

/**
 *  Defines the general memory locations.
 */

#define MEMORY_IO 0x04000000
#define MEMORY_PALETTE 0x05000000
#define MEMORY_VIDEORAM 0x06000000
#define MEMORY_OBJECT_ATTRIBUTES 0x07000000

#define REGISTER_BASE MEMORY_IO

#define BIT_FIELD_PREP(x, name) (((x) << name##_SHIFT) & name##_MASK)
#define BIT_FIELD_GET(y, name) (((y)&name##_MASK) >> name##_SHIFT)
#define BIT_FIELD_SET(y, x, name) (y = ((y) & ~name##_MASK) | BIT_FIELD_PREP(x, name))

typedef u16 Color;

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160

#define REGISTER_DISPLAY_CONTROL *(vu32*)(REGISTER_BASE + 0x0000)
#define REGISTER_VERTICAL_COUNT *(vu16*)(REGISTER_BASE + 0x0006)

#define FLAG_MODE0 0

/**
 * Wait till the background has been rendered and it is safe to modify it.
 **/
INLINE void videoSync() {
    while (REGISTER_VERTICAL_COUNT >= 160)
        ;
    while (REGISTER_VERTICAL_COUNT < 160)
        ;
}

typedef u16 ScreenEntry;

typedef struct {
    u32 data[8];
} Tile;

typedef ScreenEntry ScreenBlock[1024];
typedef Tile CharBlock[512];

#define MEMORY_BACKGROUND_PALETTE ((Color*)MEMORY_PALETTE)
#define MEMORY_BACKGROUND_PALETTE_SIZE 0x00200

#define MEMORY_SCREEN_BLOCK ((ScreenBlock*)MEMORY_VIDEORAM)
#define MEMORY_CHAR_BLOCK ((CharBlock*)MEMORY_VIDEORAM)

#define REGISTER_BACKGROUND_CONTROL ((vu16*)(REGISTER_BASE + 0x0008))
#define REGISTER_BACKGROUND_OFFSET ((BackgroundPoint*)(REGISTER_BASE + 0x0010))

#define FLAG_BACKGROUND_4BPP 0
#define FLAG_BACKGROUND_8BPP 0x0080
#define FLAG_BACKGROUND_REGULAR_32x32 0
#define FLAG_BACKGROUND_REGULAR_64x32 0x4000
#define FLAG_BACKGROUND_REGULAR_32x64 0x8000
#define FLAG_BACKGROUND_REGULAR_64x64 0xC000

#define MASK_FLAG_BACKGROUND_CHAR_BLOCK 0x000C
#define SHIFT_FLAG_BACKGROUND_CHAR_BLOCK 2
#define FLAG_BACKGROUND_CHAR_BLOCK(n) ((n) << SHIFT_FLAG_BACKGROUND_CHAR_BLOCK)

#define MASK_FLAG_BACKGROUND_SCREEN_BLOCK_MASK 0x1F00
#define SHIFT_FLAG_BACKGROUND_SCREEN_BLOCK 8
#define FLAG_BACKGROUND_SCREEN_BLOCK(n) ((n) << SHIFT_FLAG_BACKGROUND_SCREEN_BLOCK)

#define FLAG_BACKGROUND(n) ((0x0100) << n)
#define FLAG_BACKGROUND_PRIORITY(n) ((n * 1) & 0x0003)

void map_init_registers() {
    // turn off all backgrounds
    for (u32 layerIndex = MAX_LAYERS; layerIndex-- > 0;) {
        REGISTER_BACKGROUND_CONTROL[layerIndex] = 0;
    }

    // reset display control
    REGISTER_DISPLAY_CONTROL = 0;
}

Map map_load_from_rom(const u16* map_data) {
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
    for (u32 layerIndex = 0; layerIndex < map.numLayers; ++layerIndex) {
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

MapObject map_load_object(const u16* object_data, u16* index) {
    MapObject object;
    object.id = map_load_object_id(object_data, index);
    object.position = map_load_object_position(object_data, index);
    object.name = map_load_string(object_data, index);
    object.type = map_load_string(object_data, index);
    return object;
}

u32 map_load_object_id(const u16* idData, u16* index) {
    u32 upperID = map_pop_value(idData, index);
    u32 lowerID = map_pop_value(idData, index);
    return (upperID << 16) | lowerID;
}

ObjectPoint map_load_object_position(const u16* positionData, u16* index) {
    ObjectPoint position;
    position.x = map_pop_value(positionData, index);
    position.y = map_pop_value(positionData, index);
    return position;
}

const char* map_load_string(const u16* stringData, u16* index) {
    u16 length = map_pop_value(stringData, index);
    const char* string = (const char*)&stringData[*index];
    *index += length;
    return string;
}

u16 map_pop_value(const u16* data, u16* index) {
    u16 value = data[*index];
    *index += 1;
    return value;
}

void map_set_onscreen(Map map) {
    memcpy(MEMORY_BACKGROUND_PALETTE, map.palette, map.paletteLength * 2);
    memcpy(&MEMORY_CHAR_BLOCK[0][0], map.tileSet, map.tileSetLength * 2);

    u32 screenBlockStep = map.tileMapLength / ENTRIES_IN_SCREEN_BLOCK;
    u32 usedBackgrounds = 0x00;
    for (u32 layerIndex = map.numLayers; layerIndex-- > 0;) {
        u32 screenBlockIndex = NUM_SCREEN_BLOCKS - screenBlockStep * (layerIndex + 1);
        memcpy(&MEMORY_SCREEN_BLOCK[screenBlockIndex][0], map.tileMapLayers[layerIndex], map.tileMapLength * 2);

        REGISTER_BACKGROUND_CONTROL[layerIndex] |=
            FLAG_BACKGROUND_CHAR_BLOCK(0) | FLAG_BACKGROUND_SCREEN_BLOCK(screenBlockIndex) | FLAG_BACKGROUND_8BPP |
            map.sizeFlag | FLAG_BACKGROUND_PRIORITY(map.numLayers - layerIndex);
        usedBackgrounds |= FLAG_BACKGROUND(layerIndex);
    }

    REGISTER_DISPLAY_CONTROL |= FLAG_MODE0 | usedBackgrounds;
}

void map_shift(Map map, BackgroundPoint offset) {
    for (u16 layer = 0; layer < map.numLayers; layer++)
        map_shift_layer(layer, offset);
}

void map_shift_layer(u16 layer, BackgroundPoint offset) { REGISTER_BACKGROUND_OFFSET[layer] = offset; }

void map_shift_objects(MapObject* objects, ObjectPoint shift, u32 count) {
    for (u32 index = 0; index < count; index++) {
        objects[index].position.x -= shift.x;
        objects[index].position.y -= shift.y;
    }
}