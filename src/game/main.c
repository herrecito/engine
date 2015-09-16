//------------------------------------------------------------------------------
// Main loop
//------------------------------------------------------------------------------

#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include <GLFW/glfw3.h>

#include "lib/buffer.h"
#include "lib/color.h"
#include "lib/defs.h"
#include "lib/draw.h"
#include "lib/system.h"
#include "lib/geometry.h"
#include "lib/map.h"
#include "lib/perfgraph.h"

#include "input.h"
#include "game.h"

#define TICKRATE 60
#define TICKTIME (1000.0 / TICKRATE)


int main() {
    InitGame();

    PerfBuffer *perfbuf = CreatePerfBuffer(64);

    uint32_t last_tick = S_GetTime();
    while (!glfwWindowShouldClose(game.window)) {
        if ((S_GetTime() - last_tick) > TICKTIME) {
            last_tick = S_GetTime();

            PerfInfo info = {0};

            info.ticktime = ProcessInput();
            Scene scene = GetScene();
            info.drawtime = DrawPOV(&scene, game.buffer);
            DrawPerfGraph(game.buffer, perfbuf, TICKTIME);
            info.blittime = S_Blit(game.window, game.buffer);
            PushInfo(perfbuf, info);
        }
    }

    EndGame();
}
