#include <float.h>
#include <stdio.h>
#include <assert.h>

#include "collision.h"
#include "defs.h"
#include "geometry.h"
#include "map.h"

// We need to do check every segment, and keep the earliest collision.
//
// # Collision against the interior of a segment
//
//                                     s.end
//                                   +
//        .-----.        /          /
//   c  .'      ^'.     =          /
//     /     R /   \   /          /
//    /       /     \ /          =      v
//    |      +.--------------------------->
//    \     O  '·.  /          /
//     \          '+ I'     I +
//      '·_______·/          /
//               /          /.
//              /          /  '·.  n
//                      s /      ">
//                       /
//                      +  s.start
//
//  We need to calculate the point I where the circle c would touch the
//  segment s when moving with velocity v.
//
//  The circle first makes contact with the segment supporting line when the
//  perpendicular distance from O to the line is equal to the circle radius R.
//
//  First, we parameterize the movement of the circle center:
//
//      P(t) = O + tv, t < [0, 1]
//
//  We'll call t time (although it isn't, it's just a normal parameter).
//
//  Let's l be the supporting line of s and n the unit vector normal to l.
//  At time t0 of contact:
//
//      G_LinePointDistance(l, O) - R = t0 |v · n|
//
//  So:
//
//           G_LinePointDistance(l, O) - R
//      t0 = -----------------------------
//                     |v · n|
//
//  With this, we calculate the contact point:
//
//      I = O + t0 v + (G_GetSide(l, O) * R) n
//
//  and check if it's inside the segment.
//
//
//  # Collision against a vertex.
//
//      G_Distance(O, s.start) - R = t0 v · (s.start - O) / |s.start - O|
//
//               G_Distance(O, s.start) - R
//      t0 = -----------------------------------
//           v · (s.start - O) / |(s.start - O)|


// Checks if mob will hit p.
// If so, returns 1 and stores the distance and time to collision,
// returns 0 otherwise.
int CheckPoint(Vector p, Mobile mob, double *distance, double *t0) {
    double t = (G_Distance(mob.pos, p) - mob.radius) /
        G_Dot(mob.vel, G_Normalize(G_Sub(p, mob.pos)));

    if (t >= 0 && t <= 1) {
        if (distance) {
            *distance = G_Distance(mob.pos, p);
        }
        if (t0) {
            *t0 = t;
        }
        return 1;
    } else {
        return 0;
    }
}


int Co_CheckCollision(Map *map, Mobile mob, Collision *collision) {
    double v = G_Length(mob.vel);

    if (ISZERO(v)) return 0;

    int collisions = 0;
    Collision c = {
        .mob = mob,
        .t0 = DBL_MAX,
    };

    for (int i = 0; i < map->numwalls; i++) {
        Wall *w = &map->walls[i];
        Segment s = w->seg;
        Line l = G_SupportLine(s);
        Vector normal = G_Normal(l);
        double lp = G_LinePointDistance(l, mob.pos);

        // Skip if we are too far away from the line.
        if (lp > v + mob.radius) continue;

        // Skip if we are moving parallel to the line.
        if (G_Parallel(mob.vel, l.dir) && lp > mob.radius) continue;

        // Check for collision against the interior of the wall.
        double d, t = (lp - mob.radius) / fabs(G_Dot(mob.vel, normal));
        if (t >= 0 && t <= 1) {
            // We hit the support line.
            Vector I = G_Sub(
                    G_Sum(mob.pos, G_Scale(t, mob.vel)),
                    G_Scale(G_Side(l, mob.pos) * mob.radius, normal)
                    );

            if (G_IsPointOnSegment(s, I)) {
                // We hit the segment: we have a collision.
                d = G_Distance(mob.pos, I);
                if (t < c.t0) {
                    collisions++;
                    c.point = I;
                    c.t0 = t;
                    c.wall = w;
                    c.distance = d;
                }
                continue;
            }
        }

        // Check for collision against the start vertex.
        if (CheckPoint(s.start, mob, &d, &t)) {
            // We hit the start vertex.
            if (t < c.t0) {
                collisions++;
                c.point = s.start;
                c.t0 = t;
                c.wall = w;
                c.distance = d;
            }
            continue;
        }

        // Check for collision against the end vertex.
        if (CheckPoint(s.end, mob, &d, &t)) {
            // We hit the end vertex.
            if (t < c.t0) {
                collisions++;
                c.point = s.end;
                c.t0 = t;
                c.wall = w;
                c.distance = d;
            }
            continue;
        }
    }

    if (collisions && collision) {
        *collision = c;
    }

    return collisions;
}


Mobile MoveOnce(Map *map, Mobile mob) {
    Collision c;
    if (Co_CheckCollision(map, mob, &c)) {
        // Move all we can without colliding (a bit less)
        mob.pos = G_Sum(mob.pos, G_Scale(CLAMP(c.t0 - EPSILON, 0, 1), mob.vel));
        // Calculate remaining velocity
        Vector remaining_vel = G_Scale(CLAMP(1 - c.t0, 0, 1), mob.vel);
        // Calculate tangent to the mob circle at the collision point
        Vector tangent = G_Perpendicular(G_Sub(c.point, mob.pos));
        // Project the remaining velocity over the tangent
        mob.vel = G_Project(remaining_vel, tangent);
    } else {
        mob.pos = G_Sum(mob.pos, mob.vel);
        mob.vel = (Vector){0, 0};
    }

    return mob;
}

#define DEPTH 3

Mobile Co_Move(Map *map, Mobile mob) {
    for (int d = 0; d < DEPTH; d++) {
        if (ISZERO(G_Length(mob.vel))) return mob;

        mob = MoveOnce(map, mob);
    }

    return mob;
}


void PrintCollision(Collision c) {
    printf("Collision detected:\n");
    printf("\t[pos: (%.2f, %.2f), vel: %.2f, r: %.2f] -> (%.2f, %.2f)\n",
            c.mob.pos.x, c.mob.pos.y, G_Length(c.mob.vel), c.mob.radius,
            c.point.x, c.point.y);
    printf("\tWall: %p,\tt0: %.5f,\tdistance: %.5f\n", c.wall, c.t0, c.distance);
}
