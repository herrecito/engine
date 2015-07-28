//-----------------------------------------------------------------------------
// Functions to draw stuff
//-----------------------------------------------------------------------------

#ifndef _DRAW_
#define _DRAW_

#include "geometry.h"
#include "system.h"
#include "color.h"


//-----------------------------------------------------------------------------
// Low level
//-----------------------------------------------------------------------------
void D_DrawPixel(Buffer *b, int x, int y, Color color);
void D_DrawLine(Buffer *b, int x0, int y0, int x1, int y1, Color color);
void D_ClearBuffer(Buffer *b, Color color);

//-----------------------------------------------------------------------------
// High level
//-----------------------------------------------------------------------------
void D_DrawSegment(Buffer *b, Segment l, Color color);
void D_DrawBox(Buffer *buf, Box b, Color color);

#endif
