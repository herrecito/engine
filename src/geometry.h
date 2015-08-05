//------------------------------------------------------------------------------
// All the geometric structures and functions
//
// Assumes standard screen coordinates.
//
//    ^ z
//   /      x
//  o----->
//  |
//  |
//  v  y
//
// Positive angles from x to y (clockwise)
//------------------------------------------------------------------------------

#ifndef _GEOMETRY_
#define _GEOMETRY_

//------------------------------------------------------------------------------
// Vector
//------------------------------------------------------------------------------

typedef struct {
    double x, y;
} Vector;


// Vector equality
#define VEQ(a, b) (ARECLOSE((a).x, (b).x) && ARECLOSE((a).y, (b).y))

// -v
#define NVEC(v) ((Vector){ -(v).x, -(v).y })

// Returns a + b
Vector G_Sum(Vector a, Vector b);

// Returns a - b
Vector G_Sub(Vector a, Vector b);

// Returns a · b
double G_Dot(Vector a, Vector b);

// Returns |a x b|
double G_Cross(Vector a, Vector b);

// Returns the length of v
double G_Length(Vector v);

// Returns the length squared of v
double G_LengthSquared(Vector v);

// Returns the distance between two points represented by vectors a and b
double G_Distance(Vector a, Vector b);

// Returns the angle between a, b (It uses the cross product, so the angle will
// be positive if the smallest rotation to get from a to b is clockwise)
// Will return an angle between [-PI/2, PI/2]
double G_Angle(Vector a, Vector b);


// Transformations

// Returns v / |v|
Vector G_Normalize(Vector v);

// Returns v rotated angle radians clockwise
Vector G_Rotate(Vector v, double angle);

// Returns v rotated 90º clockwise
Vector G_Perpendicular(Vector v);

// Returns av
Vector G_Scale(double a, Vector v);

// Returns (a·b) b / |b| (a projected over the direction of b)
Vector G_Project(Vector a, Vector b);



//------------------------------------------------------------------------------
// Segment
//------------------------------------------------------------------------------

typedef struct {
    Vector start, end;
} Segment;


// Segment equality
#define SEGEQ(a, b) (VEQ((a).start, (b).start) && VEQ((a).end, (b).end))

// Returns l.start - l.end (direction vector of the segment)
Vector G_Direction(Segment l);

// Splits a given line l at a given point p, stores the resulting
// segments in a, b.
//
// Doesn't check that p is really over l.
void G_SplitSegment(Segment l, Vector p, Segment *a, Segment *b);

// Returns > 0 if p is on the *right side of l, < 0 if on the left,
// and 0 if p is over l.
//
// *The right of the vector l.start -> l.end
double G_GetSide(Segment l, Vector p);

// Returns > 0 if a is on the *right side of l, < 0 if on the left,
// and 0 if a crosses l.
//
// *The right of the vector l.start -> l.end
double G_GetSideSegment(Segment l, Segment a);

// Returns 1 if p is over the segment s.
int G_IsPointInSegment(Segment s, Vector p);

// Returns the minimum distance between point p and segment seg
double G_SegmentPointDistance(Segment seg, Vector p);

// Returns the center of segment s.
Vector G_SegmentCenter(Segment s);

// Returns segmented s translated by vector displacement d.
Segment G_TranslateSegment(Segment s, Vector d);

// Returns s rotated around the origin of coordinates.
Segment G_RotateSegment(Segment s, double angle);

// Returnss rotated around point.
Segment G_RotateSegmentAroundPoint(Segment s, double angle, Vector point);

// Returns the nearest point to p over seg.
Vector G_NearestPointOnSegment(Segment seg, Vector p);



//------------------------------------------------------------------------------
// Line
//------------------------------------------------------------------------------

typedef struct {
    Vector start, dir;
} Line;


// Line equality
#define LINEQ(a, b) (VEQ((a).start, (b).start) && VEQ((a).dir, (b).dir))



//------------------------------------------------------------------------------
// Intersections
//------------------------------------------------------------------------------

// Calculates the intersection between a Segment and a Ray.
//
// Returns 1 if there's an intersection, 0 otherwise.
// Stores the intersection point in intersection.
int G_SegmentRayIntersection(Segment seg, Line ray, Vector *intersection);

// Calculates the intersection between a Segment and a Line.
//
// Returns 1 if there's an intersection, 0 otherwise.
// Stores the intersection point in intersection.
int G_SegmentLineIntersection(Segment seg, Line line, Vector *intersection);

// Calculates the intersection between two Segments.
//
// Returns 1 if there's an intersection, 0 otherwise.
// Stores the intersection point in intersection.
int G_SegmentSegmentIntersection(Segment s1, Segment s2, Vector *intersection);

// Calculates the intersection between a Ray and a Line
//
// Returns 1 if there's an intersection, 0 otherwise.
// Stores the intersection point in intersection.
int G_RayLineIntersection(Line ray, Line line, Vector *intersection);



//------------------------------------------------------------------------------
// Box
//------------------------------------------------------------------------------

typedef struct {
    double top, bottom, left, right;
} Box;


// Returns 1 if Point p is inside Box b, 0 otherwise.
int G_PointInsideBox(Box b, Vector p);

#endif
