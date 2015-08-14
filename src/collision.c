#include <float.h>
#include <stdio.h>

#include "collision.h"
#include "defs.h"
#include "geometry.h"
#include "map.h"

// We need to do check every segment, and keep the closest collision.
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


int Co_CheckCollision(Map *map, Mobile mob, Vector *collision, double *t0) {
    double v = G_Length(mob.vel);

    if (ISZERO(v)) return 0;

    int collisionf = 0;
    Vector temp_collision;
    double temp_t0;
    double temp_distance = DBL_MAX;

    for (int i = 0; i < map->numwalls; i++) {
        Wall *w = &map->walls[i];
        Segment s = w->seg;
        Line l = G_SupportLine(s);
        Vector normal = G_Normal(l);

        // Skip if we are too far away from the line.
        if (G_LinePointDistance(l, mob.pos) > v + mob.radius) continue;

        // Skip if we are moving parallel to the line.
        if (G_Parallel(mob.vel, l.dir)
                && G_LinePointDistance(l, mob.pos) > mob.radius) continue;

        // Check for collision against the interior of the wall.
        double t = (G_LinePointDistance(l, mob.pos) - mob.radius) /
            fabs(G_Dot(mob.vel, normal));
        if (t >= 0 && t <= 1) {
            // We hit the support line.
            int side = G_Side(l, mob.pos);
            Vector I = G_Sub(
                    G_Sum(mob.pos, G_Scale(t, mob.vel)),
                    G_Scale(side * mob.radius, normal)
                    );

            if (G_IsPointOnSegment(s, I)) {
                // We hit the segment: we have a collision.
                double d = G_Distance(mob.pos, I);
                if (d < temp_distance) {
                    collisionf = 1;
                    temp_distance = d;
                    temp_collision = I;
                    temp_t0 = t;
                }
                continue;
            }
        }

        // Check for collision against the start vertex.
        double d;
        int rc = CheckPoint(s.start, mob, &d, &t);
        if (rc) {
            // We hit the start vertex.
            if (d < temp_distance) {
                collisionf = 1;
                temp_distance = d;
                temp_collision = s.start;
                temp_t0 = t;
            }
            continue;
        }

        // Check for collision against the end vertex.
        rc = CheckPoint(s.end, mob, &d, &t);
        if (rc) {
            // We hit the end vertex.
            if (d < temp_distance) {
                collisionf = 1;
                temp_distance = d;
                temp_collision = s.end;
                temp_t0 = t;
            }
            continue;
        }
    }


    if (collisionf) {
        if (collision) {
            *collision = temp_collision;
        }

        if (t0) {
            *t0 = temp_t0;
        }
    }

    return collisionf;
}


#define DT 0.05
#define MAXDEPTH 5

//  Once we have the collision point, we calculate the tangent to the mob
//  circle at collision time. This will be the sliding direction.
//
//  We move the mob close to the point where it would collide.
//
//  We then project the remainder of the velocity vector over the sliding
//  direction and call again the collision subroutine until there are no
//  collisions.
Vector Co_Move(Map *map, Mobile mob) {
    for (int depth = 0; depth < MAXDEPTH; depth++) {
        // Mobile is not moving, return its position.
        if (ISZERO(G_Length(mob.vel))) return mob.pos;

        Vector collision;
        double t0;

        // We don't collide with anything, just return pos + vel.
        if (!Co_CheckCollision(map, mob, &collision, &t0))
            return G_Sum(mob.pos, mob.vel);

        // We collide with something, update our mobile and check again.

        // Move all we can without colliding (a bit less)
        mob.pos = G_Sum(mob.pos, G_Scale(t0 - DT, mob.vel));
        // Calculate remaining velocity
        Vector remaining_vel = G_Scale(1 - (t0 - DT), mob.vel);
        // Calculate tangent to the mob circle at the collision point
        Vector tangent = G_Perpendicular(G_Sub(collision, mob.pos));
        // Project the remaining velocity over the tangent
        mob.vel = G_Project(remaining_vel, tangent);
    }

    // If we recurse too many times (shouldn't happen) return the latest
    // position calculated.
    return mob.pos;
}
