#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <SDL.h>

#include "buffer.h"
#include "collision.h"
#include "color.h"
#include "dbg.h"
#include "defs.h"
#include "draw.h"
#include "geometry.h"
#include "map.h"
#include "sprites.h"
#include "system.h"

//------------------------------------------------------------------------------
// Constants
//------------------------------------------------------------------------------

// Video
#define WIDTH 640
#define HEIGHT 400
#define FOV DEG2RAD(75)                         // Horizontal Field of View
#define NEAR 1                                  // Near clip plane distance
#define FAR 300                                 // Far clip plane distance
#define VIEW ((WIDTH / 2.0) / (tan(FOV / 2.0))) // Viewplane distance
#define WALLHEIGHT 64
#define POVHEIGHT (WALLHEIGHT / 2)  // Must be half the wall height.
const Vector SCREEN_CENTER = { WIDTH/2, HEIGHT/2 };
const Box SCREEN_BOX = { 0, HEIGHT-1, 0, WIDTH-1 };

// Engine
#define TICKRATE 60
#define TICKTIME (1000 / TICKRATE) // milliseconds

// Game
#define SPEED 6             // Max movement speed
#define ACCEL 1.5           // Movement acceleration
#define FRICTION 0.2        // Friction
#define VANG 0.1            // Turning speed
#define SENSITIVITY 0.002   // Mouse sensitivity
#define RADIUS 8            // Player radius



//------------------------------------------------------------------------------
// Globals
//------------------------------------------------------------------------------

Mobile player;

// A Tick contains all the input info needed to process one gametick.
typedef struct Tick {
    int forward;            // 1 forward, -1 backwards
    int strafe;             // 1 right, -1 left
    int turn;               // 1 clockwise, -1 anticlockwise
    int relative_mouse_x;   // > 0 clockwise, < 0 anticlockwise
} Tick;

Map *map;       // Current map
Buffer *buffer; // Video buffer

// Textures
Buffer *walltex;
Buffer *flortex;
Buffer *ceiltex;

SpriteSheet ascii;
SpriteSheet pistol;

// Flags
int fullscreenf = 0;  // Fullscreen
int mapf = 0;         // Automap

// Look-Up Tables
double ray_angle_lut[WIDTH];


//------------------------------------------------------------------------------
// Engine code
//------------------------------------------------------------------------------

void InitLUT() {
    for (int x = 0; x < WIDTH; x++) {
        ray_angle_lut[x] = atan2((x + 0.5) - (WIDTH / 2), VIEW);
    }
}


void Init() {
    InitLUT();

    // Window & buffer
    S_Init("Engine", WIDTH, HEIGHT);
    S_GrabMouse(1);
    buffer = B_CreateBuffer(WIDTH, HEIGHT);

    // Player
    player = (Mobile){
        .pos = (Vector){25, 25},
        .vel = (Vector){0, 0},
        .forward = G_Normalize((Vector){1, 1}),
        .radius = RADIUS,
    };

    // Map
    map = M_Load("level.map");

    // Textures
    flortex = S_LoadImage("floor.png");
    walltex = S_LoadImage("wall.png");
    ceiltex = S_LoadImage("ceil.png");

    ascii = SS_LoadSpriteSheet("ascii.png", 16, 16);
    pistol = SS_LoadSpriteSheet("pistol.png", 2, 3);
}


void Quit() {
    S_Quit();
    exit(0);
}


void DrawMap() {
    for (int i = 0; i < map->numwalls; i++) {
        Wall *w = &map->walls[i];
        if (!w->seen) continue;

        Segment s = w->seg;

        s = G_TranslateSegment(s, N(player.pos));
        s = G_TranslateSegment(s, SCREEN_CENTER);

        Segment cliped;
        if (G_ClipSegment(s, SCREEN_BOX, &cliped)) {
            D_DrawSegment(buffer, cliped, WHITE);
        }
    }

    D_DrawText(buffer, ascii, 10, 10,
            "Player pos: (%f, %f)", player.pos.x, player.pos.y);

    Segment s1 = {
        .start = SCREEN_CENTER,
        .end = G_Sum(SCREEN_CENTER, G_Scale(RADIUS, G_Rotate(player.forward, FOV / 2)))
    };
    Segment s2 = {
        .start = SCREEN_CENTER,
        .end = G_Sum(SCREEN_CENTER, G_Scale(RADIUS, G_Rotate(player.forward, -FOV / 2)))
    };

    D_DrawSegment(buffer, s1, GREEN);
    D_DrawSegment(buffer, s2, GREEN);
    D_DrawCircle(buffer, SCREEN_CENTER.x, SCREEN_CENTER.y, RADIUS, GREEN);
}


void DrawPOV() {
    for (int x = 0; x < WIDTH; x++) {
        double ray_angle = ray_angle_lut[x];
        double ray_cos = cos(ray_angle);
        double viewcos = VIEW / ray_cos;
        double nearcos = NEAR / ray_cos;

        Line ray = {
            .start = player.pos,
            .dir = G_Rotate(player.forward, ray_angle)
        };

        // Iterate over all the walls and use the one that's hit
        // closest to the player.
        Wall *wall = NULL;
        double distance = DBL_MAX;
        Vector hit;

        for (int i = 0; i < map->numwalls; i++) {
            Wall *w = &map->walls[i];
            Vector h;
            if (G_SegmentRayIntersection(w->seg, ray, &h)) {
                double d = G_Distance(h, player.pos);
                if (d < distance && d > nearcos) {
                    wall = w;
                    distance = d;
                    hit = h;
                }
            }
        }

        // Wall
        int col_height = 0;
        if (wall) {
            wall->seen = 1;

            col_height = viewcos * WALLHEIGHT / distance;
            // Everything is *much* easier if col_height is even.
            if (col_height & 1) col_height++;

            int top = (buffer->height - col_height) / 2;

            int texel_x = MOD((int)G_Distance(wall->seg.start, hit), walltex->width);
            for (int i = 0; i < col_height; i++) {
                int y = top + i;
                if (y < 0 || y >= HEIGHT) continue;

                int texel_y = WALLHEIGHT * i / col_height;
                uint32_t c = walltex->pixels[texel_y * walltex->width + texel_x];
                if (distance > FAR) {
                    c = C_ScaleColor(c, FAR / distance);
                }

                B_SetPixel(buffer, x, y, c);
            }
        }

        // Floor & ceiling
        for (int h = (HEIGHT - col_height) / 2; h > 0; h--) {
            double texel_distance = (POVHEIGHT * viewcos) / ((HEIGHT / 2) - h);
            Vector texel_world_pos = G_Sum(player.pos, G_Scale(texel_distance, ray.dir));

            int texel_x = MOD((int)texel_world_pos.x, flortex->width);
            int texel_y = MOD((int)texel_world_pos.y, flortex->height);

            uint32_t color = flortex->pixels[texel_y * flortex->width + texel_x];
            if (texel_distance > FAR) {
                color = C_ScaleColor(color, FAR / texel_distance);
            }
            B_SetPixel(buffer, x, HEIGHT - h, color);

            texel_x = MOD((int)texel_world_pos.x, ceiltex->width);
            texel_y = MOD((int)texel_world_pos.y, ceiltex->height);

            color = ceiltex->pixels[texel_y * ceiltex->width + texel_x];
            if (texel_distance > FAR) {
                color = C_ScaleColor(color, FAR / texel_distance);
            }
            B_SetPixel(buffer, x, h - 1, color);
        }
    }
}


void DrawGun() {
    Buffer *p = SS_GetSprite(pistol, 0, 0);
    B_BlitBuffer(buffer, p, 1.1 * SCREEN_CENTER.x, HEIGHT - p->height);
}


uint32_t Draw() {
    uint32_t start = S_GetTime();

    DrawPOV();
    DrawGun();

    if (mapf) {
        DrawMap();
    }

    return S_GetTime() - start;
}


Tick Input() {
    static int fwd, strafe, turn;

    Tick t = {0};
    SDL_GetRelativeMouseState(&t.relative_mouse_x, NULL);
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type)  {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                    case 'f':
                        fullscreenf = !fullscreenf;
                        S_Fullscreen(fullscreenf);
                        break;

                    case SDLK_UP:
                    case 'w':
                        fwd = 1;
                        break;

                    case SDLK_DOWN:
                    case 's':
                        fwd = -1;
                        break;

                    case 'a':
                        strafe= -1;
                        break;

                    case 'h':
                    case SDLK_LEFT:
                        turn = -1;
                        break;

                    case 'd':
                        strafe = 1;
                        break;

                    case 'l':
                    case SDLK_RIGHT:
                        turn = 1;
                        break;

                    case '\t':
                        mapf = !mapf;
                        break;

                    case 'q':
                        Quit();
                }
                break;

            case SDL_KEYUP:
                switch (ev.key.keysym.sym) {
                    case SDLK_UP:
                    case SDLK_DOWN:
                    case 'w':
                    case 's':
                        fwd = 0;
                        break;

                    case 'a':
                    case 'd':
                        strafe = 0;
                        break;

                    case 'h':
                    case 'l':
                    case SDLK_RIGHT:
                    case SDLK_LEFT:
                        turn = 0;
                        break;
                }
                break;

            case SDL_QUIT:
                Quit();
                break;
        }
    }

    t.forward = fwd;
    t.strafe = strafe;
    t.turn = turn;

    return t;
}


uint32_t ProcessATick(Tick t) {
    uint32_t start = S_GetTime();

    // Turning
    if (t.turn) {
        player.forward = G_Rotate(player.forward, t.turn * VANG);
    }

    if (t.relative_mouse_x) {
        player.forward = G_Rotate(player.forward, t.relative_mouse_x * SENSITIVITY);
    }


    // Acceleration
    Vector side = G_Perpendicular(player.forward);
    Vector a = G_SetLength(
            G_Sum(
                G_Scale(t.forward, player.forward),
                G_Scale(t.strafe,  side)
                ),
            ACCEL
            );

    if (t.forward || t.strafe) {
        player.vel = G_Sum(player.vel, a);
    } else {
        player.vel = G_Sub(player.vel, G_Scale(FRICTION, player.vel));
    }

    if (G_Length(player.vel) > SPEED) {
        player.vel = G_SetLength(player.vel, SPEED);
    }

    // Translation
    player.pos = Co_Move(map, player).pos;

    return S_GetTime() - start;
}


int main() {
    Init();

    uint32_t last_tick = S_GetTime();
    while (1) {
        if (S_GetTime() - last_tick > TICKTIME) {
            last_tick = S_GetTime();

            Tick t = Input();
            ProcessATick(t);
            Draw();
            S_Blit(buffer);
        }
    }

    Quit();
}
