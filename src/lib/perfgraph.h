//------------------------------------------------------------------------------
// Performance Graph
//------------------------------------------------------------------------------

#ifndef _PERFGRAPH_
#define _PERFGRAPH_

#include <stdint.h>

#include "buffer.h"

// Stores the time it took to process a tick, draw the full buffer
// and blit it in milliseconds.
typedef struct PerfInfo {
    uint32_t ticktime;
    uint32_t drawtime;
    uint32_t blittime;
} PerfInfo;


// Ring buffer that stores the lasts PerfInfos
typedef struct PerfBuffer {
    PerfInfo *buf;
    int head;
    int size;
} PerfBuffer;


PerfBuffer *CreatePerfBuffer(int size);

void PushInfo(PerfBuffer *perfbuf, PerfInfo info);

// Draws a performance graph.
// Each pixel represents a millisecond.
//
// Blue:    Time to process a Tick.
// Green:   Time to draw the full buffer.
// Yellow:  Time to blit the buffer to the screen.
// Red:     Maximum time per Tick available.
void DrawPerfGraph(Buffer *buf, PerfBuffer *perfbuf, int warn);

#endif
