#include "gbamap_object.h"
#include "./types.h"

u16 map_pop_value(const u16* data, u16* index) {
    u16 value = data[*index];
    *index += 1;
    return value;
}