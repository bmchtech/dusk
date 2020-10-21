#ifndef MAP_OBJECT_H
#define MAP_OBJECT_H

typedef struct {
    unsigned short x, y;
} __attribute__((aligned(4))) ObjectPoint;

typedef struct
{
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

#endif