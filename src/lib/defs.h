#ifndef _DEFS_
#define _DEFS_

#include <math.h>

#define EPSILON 0.000001

#define RAD2DEG(x) ((x) * 180.0 / M_PI)
#define DEG2RAD(x) ((x) * M_PI / 180.0)
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define CLAMP(x, min, max) (MIN(MAX((x), (min)), (max)))
#define ISZERO(f) (fabs((f)) < EPSILON)
#define EQ(a, b) (ISZERO((a) - (b)))
#define SIGN(a) (ISZERO(a) ? 0 : (a) > 0 ? 1 : -1)
#define MOD(a, b) ((((a) % (b)) + (b)) % (b))

#define LEN(ary) (sizeof(ary) / sizeof((ary)[0]))

#if !defined(NULL)
#define NULL ((void *)0)
#endif

#endif
