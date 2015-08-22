//------------------------------------------------------------------------------
// RGB color space
//------------------------------------------------------------------------------
#ifndef _COLOR_
#define _COLOR_

#include <stdint.h>

enum Color {
    BLACK     = 0x000000,
    BLUE      = 0x0000FF,
    GREEN     = 0x00FF00,
    GREY      = 0x111111,
    LIGHTGREY = 0x222222,
    RED       = 0xFF0000,
    WHITE     = 0xFFFFFF,
    YELLOW    = 0xFFFF00,

    TRANSPARENT = 0x00FFFF
};

#define GETR(c) (((c) >> 16) & 0xFF)
#define GETG(c) (((c) >>  8) & 0xFF)
#define GETB(c) (((c) >>  0) & 0xFF)

#define BUILDRGB(r, g, b) ((r) << 16 | (g) << 8 | (b))

uint32_t C_ScaleColor(uint32_t color, double intensity);

#endif
