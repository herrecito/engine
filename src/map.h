#ifndef _MAP_
#define _MAP_

#include "geometry.h"

typedef struct {
    Segment seg;

    int seen;
} Wall;

typedef struct Map {
    Wall *walls;
    int numwalls;
} Map;


Map *M_Load(const char *path);

#endif
