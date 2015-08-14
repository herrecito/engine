#ifndef _DEFS_
#define _DEFS_

#include <math.h>

// Math constants
#define PI 3.14159265359
#define EPSILON 0.000001

// Utils
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MIN(MAX((x), (min)), (max)))
#define ISZERO(f) (fabs((f)) < EPSILON ? 1 : 0)
#define EQ(a, b) (ISZERO((a) - (b)))
#define SIGN(a) (ISZERO(a) ? 0 : (a) > 0 ? 1 : -1)
#define MOD(a, b) ((((a) % (b)) + (b)) % (b))

#if !defined(NULL)
#define NULL ((void *)0)
#endif

#endif
