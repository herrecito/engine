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

typedef Vector Point;


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

// Returns 1 if p is over the segment s
int G_PointInSegment(Segment s, Vector p);

// Returns the minimum distance between point p and segment seg
double G_SegmentPointDistance(Segment seg, Vector p);



//------------------------------------------------------------------------------
// Line
//------------------------------------------------------------------------------

typedef struct {
    Vector start, dir;
} Line;

typedef Line Ray;



//------------------------------------------------------------------------------
// Intersections
//------------------------------------------------------------------------------

// Calculates the intersection between a Segment and a Ray.
//
// Returns 1 if there's an intersection, 0 otherwise.
// Stores the intersection point in p.
int G_SegmentRayIntersection(Segment seg, Ray ray, Vector *intersection);

// Calculates the intersections between a Segment and a Line.
//
// Returns 1 if there's an intersection, 0 otherwise.
// Stores the intersection point in p.
int G_SegmentLineIntersection(Segment seg, Line line, Vector *intersection);

// Calculates the intersection between two Segments.
//
// Returns 1 if there's an intersection, 0 otherwise.
// Stores the intersection point in p.
int G_SegmentSegmentIntersection(Segment s1, Segment s2, Vector *intersection);

// Returns the nearest point to p over seg.
Vector G_NearestPointOnSegment(Segment seg, Vector p);

// TODO
int G_RayLineIntersection(Ray ray, Line line, Vector *intersection);



//------------------------------------------------------------------------------
// Box
//------------------------------------------------------------------------------

typedef struct {
    double top, bottom, left, right;
} Box;


// Returns 1 if Point p is inside Box b, 0 otherwise.
int G_PointInsideBox(Box b, Point p);

#endif
