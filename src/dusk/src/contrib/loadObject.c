#include "gbamap_object.h"
#include "./types.h"

MapObject map_load_object(const u16* object_data, u16* index) {
    MapObject object;
    object.id = map_load_object_id(object_data, index);
    object.position = map_load_object_position(object_data, index);
    object.name = map_load_string(object_data, index);
    object.type = map_load_string(object_data, index);
    return object;
}