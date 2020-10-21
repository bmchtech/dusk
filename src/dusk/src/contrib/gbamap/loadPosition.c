#include "gbamap_object.h"
#include "./types.h"

ObjectPoint map_load_object_position(const u16* positionData, u16* index) {
    ObjectPoint position;
    position.x = map_pop_value(positionData, index);
    position.y = map_pop_value(positionData, index);
    return position;
}