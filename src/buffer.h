//------------------------------------------------------------------------------
// Buffers are rectangular arrays of pixels.
//------------------------------------------------------------------------------
#ifndef _BUFFER_
#define _BUFFER_

#include "color.h"

typedef struct Buffer {
    int width, height;
    uint32_t *pixels;
} Buffer;


// Returns a Buffer of the given width and height.
Buffer *B_CreateBuffer(int width, int height);

// Free a Buffer
void B_DeleteBuffer(Buffer *buf);

// Fills a Buffer with color
void B_ClearBuffer(Buffer *b, uint32_t color);

// Returns a rectangular sub-buffer of buf starting at pixel (x,y) and with
// the given width and height.
Buffer *B_GetSubBuffer(Buffer *buf, int x, int y, int width, int height);

// Copies src to dest starting at (x,y) pixel of dest.
void B_BlitBuffer(Buffer *dest, Buffer *src, int x, int y);

// Sets pixel (x,y) of b to color.
void B_SetPixel(Buffer *b, int x, int y, uint32_t color);

// Returns the color of pixel (x,y) of b.
uint32_t B_GetPixel(Buffer *b, int x, int y);

#endif
