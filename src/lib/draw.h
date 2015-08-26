//-----------------------------------------------------------------------------
// Functions to draw stuff
//-----------------------------------------------------------------------------

#ifndef _DRAW_
#define _DRAW_

#include <stdint.h>

#include "buffer.h"
#include "color.h"
#include "geometry.h"
#include "map.h"
#include "sprites.h"

void D_DrawLine(Buffer *b, int x0, int y0, int x1, int y1, uint32_t color);
void D_DrawCircle(Buffer *b, int x0, int y0, int radius, uint32_t color);
void D_DrawSegment(Buffer *b, Segment l, uint32_t color);
void D_DrawBox(Buffer *buf, Box b, uint32_t color);
void
D_DrawText(Buffer *b, SpriteSheet ascii, int x, int y, const char *fmt, ...);
uint32_t DrawPOV(Scene *sce, Buffer *buf);

#endif
