#include <stdlib.h>
#include <assert.h>

#include "buffer.h"
#include "color.h"
#include "dbg.h"

Buffer *B_CreateBuffer(int width, int height) {
    Buffer *b = malloc(sizeof(Buffer));

    b->width = width;
    b->height = height;

    b->pixels = malloc(4 * width * height);

    return b;
}


void B_ClearBuffer(Buffer *b, uint32_t color) {
    for (int i = 0; i < b->width * b->height; i++) {
        b->pixels[i] = color;
    }
}


void B_SetPixel(Buffer *b, int x, int y, uint32_t color) {
#ifndef NDEBUG
    if (!(x >= 0 && x < b->width && y >= 0 && y < b->height)) {
        debug("Drawing outside the buffer! (%d, %d)", x, y);
    }
#endif

    b->pixels[y * b->width + x] = color;
}


uint32_t B_GetPixel(Buffer *b, int x, int y) {
    return b->pixels[y * b->width + x];
}


Buffer *B_GetSubBuffer(Buffer *buf, int x, int y, int width, int height) {
    assert(x + width <= buf->width);
    assert(y + height <= buf->height);

    Buffer *b = B_CreateBuffer(width, height);

    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            B_SetPixel(b, i, j, B_GetPixel(buf, x + i, y + j));
        }
    }

    return b;
}


void B_BlitBuffer(Buffer *dest, Buffer *src, int x, int y) {
    assert(x + src->width <= dest->width);
    assert(y + src->height <= dest->height);

    for (int j = 0; j < src->height; j++) {
        for (int i = 0; i < src->width; i++) {
            B_SetPixel(dest, i + x, j + y, B_GetPixel(src, i, j));
        }
    }
}


void B_DeleteBuffer(Buffer *buf) {
    free(buf->pixels);
    free(buf);
}
