//-----------------------------------------------------------------------------
// Functions to draw stuff
//-----------------------------------------------------------------------------

#ifndef _DRAW_
#define _DRAW_

#include "geometry.h"
#include "system.h"
#include "color.h"

void D_DrawLine(Buffer *b, int x0, int y0, int x1, int y1, uint32_t color);
void D_DrawSegment(Buffer *b, Segment l, uint32_t color);
void D_DrawBox(Buffer *buf, Box b, uint32_t color);

#endif
