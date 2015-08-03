#include <stdlib.h>

#include "buffer.h"
#include "color.h"

Buffer *B_CreateBuffer(int width, int height) {
    Buffer *b = malloc(sizeof(Buffer));

    b->width = width;
    b->height = height;

    b->pixels = malloc(4 * width * height);

    return b;
}


void B_DeleteBuffer(Buffer *buf) {
    free(buf->pixels);
    free(buf);
}
