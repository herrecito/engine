#include <stdlib.h>
#include <assert.h>

#include "buffer.h"
#include "color.h"

Buffer *B_CreateBuffer(int width, int height) {
    Buffer *b = malloc(sizeof(Buffer));

    b->width = width;
    b->height = height;

    b->pixels = malloc(sizeof(uint32_t) * width * height);
    memset(b->pixels, 0, sizeof(uint32_t) * width * height);

    return b;
}


void B_ClearBuffer(Buffer *b, uint32_t color) {
    for (int i = 0; i < b->width * b->height; i++) {
        b->pixels[i] = color;
    }
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
            uint32_t src_pixel = B_GetPixel(src, i, j);
            if ((src_pixel & RGB_MASK) != TRANSPARENT) {
                B_SetPixel(dest, i + x, j + y, src_pixel);
            }
        }
    }
}


void B_DeleteBuffer(Buffer *buf) {
    free(buf->pixels);
    free(buf);
}
