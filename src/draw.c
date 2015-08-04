#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "geometry.h"
#include "defs.h"
#include "buffer.h"
#include "system.h"
#include "draw.h"
#include "dbg.h"
#include "color.h"


// Doom's version of Bresenham
void D_DrawLine(Buffer *b, int x0, int y0, int x1, int y1, uint32_t color) {
    int dx = x1 - x0;
    int ax = 2 * abs(dx);
    int sx = dx < 0 ? -1 : 1;

    int dy = y1 - y0;
    int ay = 2 * abs(dy);
    int sy = dy < 0 ? -1 : 1;

    int x = x0;
    int y = y0;

    if (ax > ay) {
        int d = ay - ax / 2;
        while (1) {
            B_SetPixel(b, x, y, color);
            if (x == x1) return;

            if (d >= 0) {
                y += sy;
                d -= ax;
            }
            x += sx;
            d += ay;
        }
    } else {
        int d = ax - ay / 2;
        while (1) {
            B_SetPixel(b, x, y, color);
            if (y == y1) return;

            if (d >= 0) {
                x += sx;
                d -= ay;
            }
            y += sy;
            d += ax;
        }
    }
}



void D_DrawSegment(Buffer *b, Segment l, uint32_t color) {
    D_DrawLine(b, l.start.x, l.start.y, l.end.x, l.end.y, color);
}



void D_DrawBox(Buffer *buf, Box b, uint32_t color) {
    D_DrawLine(buf, b.left, b.top, b.right, b.top, color);
    D_DrawLine(buf, b.left, b.bottom, b.right, b.bottom, color);
    D_DrawLine(buf, b.left, b.top, b.left, b.bottom, color);
    D_DrawLine(buf, b.right, b.top, b.right, b.bottom, color);
}
