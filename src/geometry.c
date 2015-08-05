#include <math.h>
#include <stdlib.h>

#include "defs.h"
#include "geometry.h"


double G_Length(Vector v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

double G_LengthSquared(Vector v) {
    return v.x * v.x + v.y * v.y;
}


Vector G_Direction(Segment l) {
    return (Vector){ l.end.x - l.start.x, l.end.y - l.end.x };
}


// Splits a given line l at a given point p, stores the resulting
// segments in a, b.
//
// Doesn't check that p is really over l.
void G_SplitSegment(Segment l, Vector p, Segment *a, Segment *b) {
    a->start = l.start;
    a->end = p;
    b->start = p;
    b->end = l.end;
}


// Returns > 0 if p is on the *right side of l, < 0 if on the left,
// and 0 if p is over l.
//
// *The right of the vector l.start -> l.end
double G_GetSide(Segment l, Vector p) {
    return G_Cross(G_Sub(l.end, l.start), G_Sub(p, l.start));
}

// Returns > 0 if a is on the *right side of l, < 0 if on the left, and 0 if a
// crosses l;
//
// *The right of the vector l.start -> l.end
double G_GetSideSegment(Segment l, Segment a) {
    double side_a_start = G_GetSide(l, a.start);
    double side_a_end = G_GetSide(l, a.end);

    if (SIGN(side_a_start) == SIGN(side_a_end)) {
        return side_a_start;
    } else {
        return 0;
    }
}


double G_Dot(Vector a, Vector b) {
    return a.x * b.x + a.y * b.y;
}


Vector G_Project(Vector a, Vector b) {
    return G_Scale(G_Dot(a, b), G_Normalize(b));
}


Vector G_Rotate(Vector v, double angle) {
    Vector r = {
        v.x * cos(angle) - v.y * sin(angle),
        v.x * sin(angle) + v.y * cos(angle)
    };

    return r;
}

double G_Cross(Vector a, Vector b) {
    return a.x * b.y - a.y * b.x;
}

Vector G_Normalize(Vector v) {
    double l = G_Length(v);
    return (Vector){ v.x / l, v.y / l };
}


Vector G_Sum(Vector a, Vector b) {
    return (Vector){ a.x + b.x, a.y + b.y };
}


Vector G_Sub(Vector a, Vector b) {
    return (Vector){ a.x - b.x, a.y - b.y };
}


Vector G_Scale(double a, Vector v) {
    return (Vector){ a * v.x, a * v.y };
}

double G_Angle(Vector a, Vector b) {
    return asin(G_Cross(a, b) / (G_Length(a) * G_Length(b)));
}


Vector G_Perpendicular(Vector v) {
    return (Vector){ -v.y, v.x };
}


double G_Distance(Vector a, Vector b) {
    return G_Length(G_Sub(b, a));
}


// Calculate intersection between a Segment and a Ray.
//
// Returns 1 if there's an intersection, 0 otherwise.
// Stores the intersection point in p.
//
// Math explanation (It's the same for the others intersections)
//
//        ^ r
// A     /       B
// ·----/--------·
//     /
//    /
// P ·
//
// L(t) = A + t(B-A), t < [0, 1]
// R(t) = P + s(r), s < [0, inf)
//
// L(t) = R(t); ...
//
// t = ((P - A) x r) / ((B - A) x r)
// s = ((B - A) x (P - A)) / (r x (B - A))
int G_SegmentRayIntersection(Segment seg, Line ray, Vector *intersection) {
    Vector ab = G_Sub(seg.end, seg.start);
    Vector ap = G_Sub(ray.start, seg.start);
    Vector r = ray.dir;

    double t = G_Cross(ap, r) / G_Cross(ab, r);
    double s = G_Cross(ab, ap) / G_Cross(r, ab);

    if (t >= 0 && t <= 1 && s >= 0) {
        *intersection = G_Sum(ray.start, G_Scale(s, r));

        return 1;
    } else {
        return 0;
    }
}

int G_IsPointInSegment(Segment s, Vector p) {
    Vector ab = G_Sub(s.end, s.start);
    Vector ap = G_Sub(p, s.start);

    if (!ISZERO(G_Cross(ab, ap))) return 0;

    double dot = G_Dot(ab, ap);
    if (dot < 0) return 0;

    double l = G_LengthSquared(ab);
    if (dot > l) return 0;

    return 1;
}


double G_SegmentPointDistance(Segment seg, Vector p) {
    double ap = G_Distance(p, seg.start);
    double bp = G_Distance(p, seg.end);

    Line perpendicular = {
        .start = p,
        .dir = G_Perpendicular(G_Direction(seg))
    };

    Vector p_l;
    if (G_SegmentLineIntersection(seg, perpendicular, &p_l)) {
        return MIN(MIN(ap, bp), G_Distance(p, p_l));
    }

    return MIN(ap, bp);
}


Vector G_NearestPointOnSegment(Segment seg, Vector p) {
    double ap = G_Distance(p, seg.start);
    double bp = G_Distance(p, seg.end);

    Line perpendicular = {
        .start = p,
        .dir = G_Perpendicular(G_Direction(seg))
    };

    Vector p_l;
    if (G_SegmentLineIntersection(seg, perpendicular, &p_l)) {
        double ppl = G_Distance(p, p_l);
        if (ppl < ap && ppl < bp) {
            return p_l;
        }
    }

    if (ap < bp) {
        return seg.start;
    } else {
        return seg.end;
    }
}


// Calculate intersection between a Segment and a Line.
//
// Returns 1 if there's an intersection, 0 otherwise.
// Stores the intersection point in p.
int G_SegmentLineIntersection(Segment seg, Line line, Vector *intersection) {
    Vector ab = G_Sub(seg.end, seg.start);
    Vector ap = G_Sub(line.start, seg.start);
    Vector r = line.dir;

    double t = G_Cross(ap, r) / G_Cross(ab, r);

    if (t >= 0 && t <= 1) {
        if (intersection) {
            *intersection = G_Sum(seg.start, G_Scale(t, ab));
        }

        return 1;
    } else {
        return 0;
    }
}

int G_SegmentSegmentIntersection(Segment s1, Segment s2, Vector *intersection) {
    Vector ab = G_Sub(s1.end, s1.start);
    Vector cd = G_Sub(s2.end, s2.start);
    Vector ac = G_Sub(s2.start, s1.start);
    Vector ca = G_Sub(s1.start, s2.start);

    double t = G_Cross(ac, cd) / G_Cross(ab, cd);
    double s = G_Cross(ca, ab) / G_Cross(cd, ab);

    if (t >= 0 && t <= 1 && s >= 0 && s <= 1) {
        if (intersection) {
            *intersection = G_Sum(s1.start, G_Scale(t, ab));
        }

        return 1;
    } else {
        return 0;
    }
}


int G_RayLineIntersection(Line ray, Line line, Vector *intersection) {
    // Vector ab = G_Sub(ray.start, line.start);  // Unused
    Vector ba = G_Sub(line.start, ray.start);
    Vector v = line.dir;
    Vector w = ray.dir;

    double s = G_Cross(ba, v) / G_Cross(w, v);
    // double t = G_Cross(ab, w) / G_Cross(v, w);  // Unused

    if (s >= 0) {
        if (intersection) {
            *intersection = G_Sum(ray.start, G_Scale(s, ray.dir));
        }

        return 1;
    } else {
        return 0;
    }
}

int G_PointInsideBox(Box b, Vector p) {
    return p.x < b.right && p.x > b.left && p.y < b.bottom && p.y > b.top;
}


Segment G_TranslateSegment(Segment s, Vector d) {
    return (Segment){ .start = G_Sum(s.start, d), .end = G_Sum(s.end, d) };
}


Vector G_SegmentCenter(Segment s) {
    return (Vector){ (s.start.x + s.end.x) / 2, (s.start.y + s.end.y) / 2 };
}


Segment G_RotateSegment(Segment s, double angle) {
    return (Segment){
        .start = G_Rotate(s.start, angle),
        .end = G_Rotate(s.end, angle)
    };
}

Segment G_RotateSegmentAroundPoint(Segment s, double angle, Vector point) {
    Segment temp = G_TranslateSegment(s, NVEC(point));

    temp = G_RotateSegment(temp, angle);

    return G_TranslateSegment(temp, point);
}


// Regions:
//
// 1001 | 1000 | 1010
// -----|------|-----
// 0001 | 0000 | 0010
// -----|------|-----
// 0101 | 0100 | 0110

enum Region {
    G_REGION_LEFT   = 1 << 0,
    G_REGION_RIGHT  = 1 << 1,
    G_REGION_BOTTOM = 1 << 2,
    G_REGION_TOP    = 1 << 3,
};

int FindRegion(Box rect, Vector p) {
    int code = 0;

    if (p.y < rect.top && !ARECLOSE(p.y, rect.top)) code |= G_REGION_TOP;
    if (p.y > rect.bottom && !ARECLOSE(p.y, rect.bottom)) code |= G_REGION_BOTTOM;
    if (p.x > rect.right && !ARECLOSE(p.x, rect.right))  code |= G_REGION_RIGHT;
    if (p.x < rect.left && !ARECLOSE(p.x, rect.left)) code |= G_REGION_LEFT;

    return code;
}


// Cohen Sutherland inspired clipping.
int G_ClipSegment(Segment in, Box rect, Segment *out) {
    int start_region = FindRegion(rect, in.start);
    int end_region = FindRegion(rect, in.end);

    // Trivial accept
    // Both endpoints inside rect.
    if ((start_region | end_region) == 0) {
        *out = in;
        return 1;
    }

    // Trivial reject
    // Both endpoints are on the same side of rect (e.g. both above)
    if ((start_region & end_region)) return 0;

    // No trivial case, let's clip.
    Line left =   { .start = {rect.left, rect.top},    .dir = {0, 1} };
    Line right =  { .start = {rect.right, rect.top},   .dir = {0, 1} };
    Line bottom = { .start = {rect.left, rect.bottom}, .dir = {1, 0} };
    Line top =    { .start = {rect.left, rect.top},    .dir = {1, 0} };

    Segment new = in;

    if (start_region & G_REGION_TOP) {
        G_SegmentLineIntersection(new, top, &new.start);
        start_region = FindRegion(rect, new.start);
    } else if (start_region & G_REGION_BOTTOM) {
        G_SegmentLineIntersection(new, bottom, &new.start);
        start_region = FindRegion(rect, new.start);
    }

    if (start_region & G_REGION_LEFT) {
        G_SegmentLineIntersection(new, left, &new.start);
        start_region = FindRegion(rect, new.start);
    } else if (start_region & G_REGION_RIGHT) {
        G_SegmentLineIntersection(new, right, &new.start);
        start_region = FindRegion(rect, new.start);
    }

    if (end_region & G_REGION_TOP) {
        G_SegmentLineIntersection(new, top, &new.end);
        end_region = FindRegion(rect, new.end);
    } else if (end_region & G_REGION_BOTTOM) {
        G_SegmentLineIntersection(new, bottom, &new.end);
        end_region = FindRegion(rect, new.end);
    }

    if (end_region & G_REGION_LEFT) {
        G_SegmentLineIntersection(new, left, &new.end);
        end_region = FindRegion(rect, new.end);
    } else if (end_region & G_REGION_RIGHT) {
        G_SegmentLineIntersection(new, right, &new.end);
        end_region = FindRegion(rect, new.end);
    }

    // Again

    // Trivial accept
    // Both endpoints inside rect.
    if ((start_region | end_region) == 0) {
        *out = new;
        return 1;
    }

    // Trivial reject
    // Both endpoints are on the same side of rect (e.g. both above)
    if ((start_region & end_region)) return 0;

    return 0;  // Shouldn't get here
}
