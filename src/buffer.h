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


#endif
