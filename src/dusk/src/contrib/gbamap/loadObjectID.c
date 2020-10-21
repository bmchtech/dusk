#include "gbamap_object.h"
#include "./types.h"

u32 map_load_object_id(const u16* idData, u16* index) {
    u32 upperID = map_pop_value(idData, index);
    u32 lowerID = map_pop_value(idData, index);
    return (upperID << 16) | lowerID;
}