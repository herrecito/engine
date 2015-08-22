#include <string.h>
#include <stdarg.h>
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


// https://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void D_DrawCircle(Buffer *b, int x0, int y0, int radius, uint32_t color) {
    int x = radius;
    int y = 0;
    int decisionOver2 = 1 - x;  // Decision criterion divided by 2 evaluated at x=r, y=0

    while (x >= y) {
        B_SetPixel(b,  x + x0,  y + y0, color);
        B_SetPixel(b,  y + x0,  x + y0, color);
        B_SetPixel(b, -x + x0,  y + y0, color);
        B_SetPixel(b, -y + x0,  x + y0, color);
        B_SetPixel(b, -x + x0, -y + y0, color);
        B_SetPixel(b, -y + x0, -x + y0, color);
        B_SetPixel(b,  x + x0, -y + y0, color);
        B_SetPixel(b,  y + x0, -x + y0, color);
        y++;

        if (decisionOver2 <= 0) {
            decisionOver2 += 2 * y + 1;  // Change in decision criterion for y -> y+1
        } else {
            x--;
            decisionOver2 += 2 * (y - x) + 1;  // Change for y -> y+1, x -> x-1
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


void
D_DrawText(Buffer *b, SpriteSheet ascii, int x, int y, const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    char text[128] = "";
    vsnprintf(text, 128, fmt, args);

    va_end(args);

    for (int i = 0; i < strlen(text); i++) {
        unsigned char c = text[i];

        int xx = c % 16;
        int yy = c / 16;

        Buffer *bitmap = SS_GetSprite(ascii, xx, yy);

        B_BlitBuffer(b, bitmap, x + i * 8, y);
    }
}
