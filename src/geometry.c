#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include "defs.h"
#include "geometry.h"


double G_Length(Vector v) {
    return sqrt(v.x * v.x + v.y * v.y);
}


double G_LengthSquared(Vector v) {
    return v.x * v.x + v.y * v.y;
}


Vector G_Dir(Segment l) {
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


int G_Side(Line l, Vector p) {
    return SIGN(G_Cross(l.dir, G_Sub(p, l.start)));
}


double G_Dot(Vector a, Vector b) {
    return a.x * b.x + a.y * b.y;
}


Vector G_Project(Vector a, Vector b) {
    Vector dir = G_Normalize(b);
    return G_Scale(G_Dot(a, dir), dir);
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
    if (ISZERO(l)) return (Vector){0, 0};
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


Vector G_SetLength(Vector v, double l) {
    return G_Scale(l, G_Normalize(v));
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

    if (G_Parallel(ab, r)) return 0;

    double t = G_Cross(ap, r) / G_Cross(ab, r);
    double s = G_Cross(ab, ap) / G_Cross(r, ab);

    if (t >= 0 && t <= 1 && s >= 0) {
        if (intersection) {
            *intersection = G_Sum(ray.start, G_Scale(s, r));
        }

        return 1;
    } else {
        return 0;
    }
}


int G_IsPointOnSegment(Segment s, Vector p) {
    Vector ab = G_Sub(s.end, s.start);
    Vector ap = G_Sub(p, s.start);

    if (!G_Parallel(ab, ap)) return 0;

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
        .dir = G_Perpendicular(G_Dir(seg))
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
        .dir = G_Perpendicular(G_Dir(seg))
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


int G_SegmentLineIntersection(Segment seg, Line line, Vector *intersection) {
    Vector ab = G_Sub(seg.end, seg.start);
    Vector ap = G_Sub(line.start, seg.start);
    Vector r = line.dir;

    if (G_Parallel(ab, r)) return 0;

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

    if (G_Parallel(ab, cd)) return 0;

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
    if (G_Parallel(ray.dir, line.dir)) return 0;

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
    int left = p.x > b.left || EQ(p.x, b.left);
    int right = p.x < b.right || EQ(p.x, b.right);
    int top = p.y > b.top || EQ(p.y, b.top);
    int bottom = p.y < b.bottom || EQ(p.y, b.bottom);
    return left && right && top && bottom;
}


Segment G_TranslateSegment(Segment s, Vector d) {
    return (Segment){ .start = G_Sum(s.start, d), .end = G_Sum(s.end, d) };
}


Vector G_Center(Segment s) {
    return G_Midpoint(s.start, s.end);
}


Segment G_RotateSegment(Segment s, double angle) {
    Segment r = {
        .start = G_Rotate(s.start, angle),
        .end = G_Rotate(s.end, angle)
    };

    return r;
}


Segment G_RotateSegmentAroundPoint(Segment s, double angle, Vector point) {
    Segment temp = G_TranslateSegment(s, N(point));

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

    if (p.y < rect.top && !EQ(p.y, rect.top)) code |= G_REGION_TOP;
    if (p.y > rect.bottom && !EQ(p.y, rect.bottom)) code |= G_REGION_BOTTOM;
    if (p.x > rect.right && !EQ(p.x, rect.right))  code |= G_REGION_RIGHT;
    if (p.x < rect.left && !EQ(p.x, rect.left)) code |= G_REGION_LEFT;

    return code;
}


// Cohen Sutherland inspired clipping.
int G_ClipSegment(Segment in, Box rect, Segment *out) {
    int start_region = FindRegion(rect, in.start);
    int end_region = FindRegion(rect, in.end);

    // Trivial accept
    // Both endpoints inside rect.
    if ((start_region | end_region) == 0) {
        if (out) {
            *out = in;
        }

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
        if (out) {
            *out = new;
        }
        return 1;
    }

    // Trivial reject
    // Both endpoints are on the same side of rect (e.g. both above)
    if ((start_region & end_region)) return 0;

    return 0;  // Shouldn't get here
}


double G_LinePointDistance(Line l, Vector p) {
    Line perpendicular = {
        .start = p,
        .dir = G_Perpendicular(l.dir)
    };

    Vector intersection;
    int rc = G_LineLineIntersection(l, perpendicular, &intersection);

    assert(rc == 1);

    return G_Distance(p, intersection);
}


int G_LineLineIntersection(Line l1, Line l2, Vector *intersection) {
    if (G_Parallel(l1.dir, l2.dir)) return 0;

    // Vector ab = G_Sub(l1.start, l2.start);  // Unused
    Vector ba = G_Sub(l2.start, l1.start);
    Vector v = l2.dir;
    Vector w = l1.dir;

    double s = G_Cross(ba, v) / G_Cross(w, v);
    // double t = G_Cross(ab, w) / G_Cross(v, w);  // Unused

    if (intersection) {
        *intersection = G_Sum(l1.start, G_Scale(s, l1.dir));
    }

    return 1;
}


Line G_SupportLine(Segment seg) {
    return (Line){ .start = seg.start, .dir = G_Sub(seg.end, seg.start) };
}


Vector G_Normal(Line l) {
    return G_Normalize(G_Perpendicular(l.dir));
}


int G_Parallel(Vector a, Vector b) {
    return ISZERO(G_Cross(a, b));
}

Vector G_Midpoint(Vector a, Vector b) {
    return (Vector){(a.x + b.x) / 2, (a.y + b.y) / 2};
}
