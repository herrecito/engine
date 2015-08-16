//------------------------------------------------------------------------------
// RGB color space
//------------------------------------------------------------------------------
#ifndef _COLOR_
#define _COLOR_

#include <stdint.h>

#define BLACK     0x000000
#define BLUE      0x0000FF
#define GREEN     0x00FF00
#define GREY      0x111111
#define LIGHTGREY 0x222222
#define RED       0xFF0000
#define WHITE     0xFFFFFF
#define YELLOW    0xFFFF00

#define GETR(c) (((c) >> 16) & 0xFF)
#define GETG(c) (((c) >>  8) & 0xFF)
#define GETB(c) (((c) >>  0) & 0xFF)

#define BUILDRGB(r, g, b) ((r) << 16 | (g) << 8 | (b))

uint32_t C_ScaleColor(uint32_t color, double intensity);

#endif
