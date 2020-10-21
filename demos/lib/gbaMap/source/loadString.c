#include "gbamap_object.h"
#include "./types.h"

const char* map_load_string(const u16* stringData, u16* index) {
    u16 length = map_pop_value(stringData, index);
    const char* string = (const char*) &stringData[*index];
    *index += length;
    return string;
}