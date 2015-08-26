#ifndef _MAP_
#define _MAP_

#include "buffer.h"
#include "geometry.h"

// Represents anything that can move.
// It has a size, represented by the radius of a surrounding circle.
typedef struct Mobile {
    Vector pos;
    Vector vel;
    Vector forward;  // *Must* be a versor
    double radius;
} Mobile;

typedef struct {
    Segment seg;
} Wall;

typedef struct Map {
    Wall *walls;
    int numwalls;

    Buffer *floortex;
    Buffer *ceiltex;
    Buffer *walltex;
} Map;

typedef struct Scene {
    Mobile *pov;
    Map *map;
} Scene;


Map *M_Load(const char *path);


#endif
