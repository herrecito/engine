#include <stdlib.h>

#include "color.h"
#include "buffer.h"
#include "perfgraph.h"

PerfBuffer *CreatePerfBuffer(int size) {
    PerfBuffer *perfbuf = malloc(sizeof(PerfBuffer));

    perfbuf->size = size;
    perfbuf->head = 0;
    perfbuf->buf = calloc(1, sizeof(PerfInfo) * size);

    return perfbuf;
}


void PushInfo(PerfBuffer *perfbuf, PerfInfo info) {
    perfbuf->buf[perfbuf->head++] = info;

    if (perfbuf->head == perfbuf->size) {
        perfbuf->head = 0;
    }
}


static void DrawOneInfo(Buffer *buf, PerfInfo info, int warn, int x) {
    int y = 20;

    for (int j = 0; j < info.ticktime; j++) {
        B_SetPixel(buf, x, y--, BLUE);
    }

    for (int j = 0; j < info.drawtime; j++) {
        B_SetPixel(buf, x, y--, GREEN);
    }

    for (int j = 0; j < info.blittime; j++) {
        B_SetPixel(buf, x, y--, YELLOW);
    }

    if (warn) {
        B_SetPixel(buf, x, 20 - warn, RED);
    }
}


void DrawPerfGraph(Buffer *buf, PerfBuffer *perfbuf, int warn) {
    int x = buf->width - 4;

    for (int i = perfbuf->head- 1; i >= 0; i--, x--) {
        DrawOneInfo(buf, perfbuf->buf[i], warn, x);
    }

    for (int i = perfbuf->size - 1; i >= perfbuf->head; i--, x--) {
        DrawOneInfo(buf, perfbuf->buf[i], warn, x);
    }
}
