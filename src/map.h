#ifndef _MAP_
#define _MAP_

#include "geometry.h"

typedef struct {
    Segment seg;
    int height;
} Wall;


typedef struct Map {
    char *name;
    Vector spawn;

    Wall *walls;
    int numwalls;
} Map;


typedef struct MapIterator MapIterator;

Map *M_Load(const char *path);

MapIterator *M_GetIterator(Map *map);
Wall *M_GetNext(MapIterator *it);
void M_DeleteIterator(MapIterator *it);
void M_GoBeforeFirst(MapIterator *it);

#endif
