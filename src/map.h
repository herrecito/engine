#ifndef _MAP_
#define _MAP_

#include "geometry.h"

typedef struct {
    Segment seg;
} Wall;

typedef struct Map {
    char *name;
    Vector spawn;

    Wall *walls;
    int numwalls;
} Map;


Map *M_Load(const char *path);

#endif
