//------------------------------------------------------------------------------
// Collision detection and response
//------------------------------------------------------------------------------
#ifndef _COLLISION_
#define _COLLISION_

#include "map.h"
#include "geometry.h"

// Represents anything that can move and collide with walls.
// It has a size, represented by the radius of a surrounding circle.
typedef struct Mobile {
    Vector pos;
    Vector vel;
    double radius;
} Mobile;


// Returns the final position of mob in map after handling collisions.
Vector Co_Move(Map *map, Mobile mob);

// Checks if mob will hit anything in map.
//
// Returns 1 and stores the collision point and time to collision t0 if
// there's a collision.
//
//                  mob.radius
//                 <----------->
// mob.pos
// |                   mob.vel
// +---------------+---------->+        t0 < [0, 1]
//                 |           |
//                 t0 mob.vel  collision
//
// Returns 0 otherwise.
int Co_CheckCollision(Map *map, Mobile mob, Vector *collision, double *t0);

#endif
