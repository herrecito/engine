#include <SDL.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "buffer.h"
#include "color.h"
#include "dbg.h"
#include "defs.h"
#include "draw.h"
#include "geometry.h"
#include "map.h"
#include "system.h"
#include "sprites.h"


// Video constants
#define WIDTH 320
#define HEIGHT 200
#define FOV (PI / 2.5)
#define NEAR 10                                 // Near clip plane distance
#define FAR 300
#define VIEW ((WIDTH / 2.0) / (tan(FOV / 2.0))) // Viewplane distance
#define VFOV (atan2(HEIGHT / 2.0, VIEW))

#define WALLHEIGHT 64
#define POVHEIGHT (WALLHEIGHT / 2)

// Game constants
#define TICKRATE 60
#define TICKTIME (1000 / TICKRATE) // milliseconds
#define FPS 60
#define FRAMETIME (1000 / FPS)

#define VEL 3
#define VANG 0.1
#define SENSITIVITY 0.002


// Globals
struct {
    Vector position;
    Vector forward;

    int fwd, strafe, turn;
} player;

Map *map;

Buffer *buffer;

Buffer *walltex;
Buffer *flortex;
Buffer *ceiltex;
Sprites *numbers;

int grabf = 1;  // Grab mouse flag
int mapf;       // Show map flag
int walkf;      // Walk flag

const Vector SCREEN_CENTER = { WIDTH/2, HEIGHT/2 };
const Box SCREEN_BOX = { 0, HEIGHT-1, 0, WIDTH-1 };


void Init() {
    // Window & buffer
    S_Init("Engine", WIDTH*2, HEIGHT*2);
    S_GrabMouse(1);
    buffer = B_CreateBuffer(WIDTH, HEIGHT);
    B_ClearBuffer(buffer, WHITE);

    // Player
    player.position = (Vector){ 25, 25 };
    player.forward = G_Normalize((Vector){ 1, 1 });

    // Map
    map = M_Load("level.map");

    // Textures
    flortex = S_LoadTexture("floor.png");
    walltex = S_LoadTexture("wall.png");
    ceiltex = S_LoadTexture("ceil.png");
    numbers = S_LoadSprites("numbers.png", 1, 10);
}



void Quit() {
    S_Quit();
    exit(1);
}


void DrawPlayer() {
    Segment s1 = {
        .start = SCREEN_CENTER,
        .end = G_Sum(SCREEN_CENTER, G_Scale(NEAR / cos(FOV / 2), G_Rotate(player.forward, FOV / 2)))
    };
    Segment s2 = {
        .start = SCREEN_CENTER,
        .end = G_Sum(SCREEN_CENTER, G_Scale(NEAR / cos(FOV / 2), G_Rotate(player.forward, -FOV / 2)))
    };

    D_DrawSegment(buffer, s1, GREEN);
    D_DrawSegment(buffer, s2, GREEN);
}


void DrawMap() {
    for (int i = 0; i < map->numwalls; i++) {
        Wall *w = &map->walls[i];
        if (!w->seen) continue;

        Segment s = w->seg;

        s = G_TranslateSegment(s, NVEC(player.position));
        s = G_TranslateSegment(s, SCREEN_CENTER);

        Segment cliped;
        if (G_ClipSegment(s, SCREEN_BOX, &cliped)) {
            D_DrawSegment(buffer, cliped, WHITE);
        }
    }

    DrawPlayer();
}


void DrawPOV() {
    for (int x = 0; x < WIDTH; x++) {
        double ray_angle = atan2((x + 0.5) - (WIDTH / 2), VIEW);
        double ray_cos = cos(ray_angle);
        double viewcos = VIEW / ray_cos;
        double nearcos = NEAR / ray_cos;

        Line ray = {
            .start = player.position,
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
                double d = G_Distance(h, player.position);
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
            Vector texel_world_pos = G_Sum(player.position, G_Scale(texel_distance, ray.dir));

            int texel_x = MOD((int)texel_world_pos.x, flortex->width);
            int texel_y = MOD((int)texel_world_pos.y, flortex->height);

            uint32_t color = flortex->pixels[texel_y * flortex->width + texel_x];
            if (texel_distance > FAR) {
                color = C_ScaleColor(color, FAR / texel_distance);
            }
            B_SetPixel(buffer, x, HEIGHT - h, color);

            color = ceiltex->pixels[texel_y * ceiltex->width + texel_x];
            if (texel_distance > FAR) {
                color = C_ScaleColor(color, FAR / texel_distance);
            }
            B_SetPixel(buffer, x, h - 1, color);
        }
    }
}



void Draw() {
    DrawPOV();

    if (mapf) {
        DrawMap();
    }

    S_Blit(buffer);
}



void Input() {
    int x;
    SDL_GetRelativeMouseState(&x, NULL);
    if (x) {
        player.forward = G_Rotate(player.forward, x * SENSITIVITY);
    }

    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        switch (ev.type)  {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                    case 'f':
                        S_ToggleFullcreen();
                        break;

                    case SDLK_LSHIFT:
                        walkf = 1;
                        break;

                    case SDLK_UP:
                    case 'w':
                        player.fwd = 1;
                        break;

                    case SDLK_DOWN:
                    case 's':
                        player.fwd = -1;
                        break;

                    case 'a':
                        player.strafe= -1;
                        break;

                    case 'h':
                    case SDLK_LEFT:
                        player.turn = -1;
                        break;

                    case 'd':
                        player.strafe = 1;
                        break;

                    case 'l':
                    case SDLK_RIGHT:
                        player.turn = 1;
                        break;

                    case '\t':
                        mapf = !mapf;
                        break;

                    case 'g':
                        grabf = !grabf;
                        S_GrabMouse(grabf);
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
                        player.fwd = 0;
                        break;

                    case 'a':
                    case 'd':
                        player.strafe = 0;
                        break;

                    case 'h':
                    case 'l':
                    case SDLK_RIGHT:
                    case SDLK_LEFT:
                        player.turn = 0;
                        break;

                    case SDLK_LSHIFT:
                        walkf = 0;
                }
                break;
        }
    }
}



void Logic() {
    // Turning
    if (player.turn) {
        player.forward = G_Rotate(player.forward, player.turn * VANG);
    }

    // Translation
    if (player.fwd || player.strafe) {
        Vector side = G_Perpendicular(player.forward);

        int vel = walkf ? VEL / 2 : VEL;

        Vector velocity = G_Scale(vel,
                G_Normalize(
                    G_Sum(
                        G_Scale(player.fwd, player.forward),
                        G_Scale(player.strafe, side)
                        )
                    )
                );

        Vector end = G_Sum(player.position, velocity);

        player.position = end;
    }
}


int FPS_LIMIT = 0;

int main() {
    Init();

    uint32_t last_tick = SDL_GetTicks();
    uint32_t last_second = SDL_GetTicks();
    uint32_t last_frame = SDL_GetTicks();
    int fps_counter = 0;

    while (1) {
        uint32_t start = SDL_GetTicks();

        // Run ticks
        if (SDL_GetTicks() - last_tick > TICKTIME) {
            last_tick = SDL_GetTicks();

            Input();
            Logic();
        }

        // Keep mouse inside window
        S_MouseFix();

        // Draw screen
        if (FPS_LIMIT) {
            if (SDL_GetTicks() - last_frame > FRAMETIME) {
                last_frame = SDL_GetTicks();

                Draw();

                fps_counter++;
            }
        } else {
            Draw();

            fps_counter++;
        }

        // FPS Counter
        if (SDL_GetTicks() - last_second > 1000) {
            last_second = SDL_GetTicks();
            printf("FPS: %d\n", fps_counter);
            fps_counter = 0;
        }

        // Sleep if FPS_LIMIT and there's some time left.
        if (FPS_LIMIT) {
            int free_time = TICKTIME - (SDL_GetTicks() - start);
            if (free_time > 10) {
                SDL_Delay(1);  // Must assume it will sleep for 10ms;
            } else if (free_time < 0) {
                puts("Too slow!");
            }
        }
    }

    Quit();
}
