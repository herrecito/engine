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

void B_ClearBuffer(Buffer *b, uint32_t color);
Buffer *B_GetSubBuffer(Buffer *buf, int x, int y, int width, int height);
void B_BlitBuffer(Buffer *dest, Buffer *src, int x, int y);

void B_SetPixel(Buffer *b, int x, int y, uint32_t color);
uint32_t B_GetPixel(Buffer *b, int x, int y);

#endif
