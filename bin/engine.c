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
static MapIterator *it;

Buffer *buffer;

Texture *wall;
Texture *flor;
Texture *ceiling;

int grabf = 1;  // Grab mouse flag
int mapf;       // Show map flag
int walkf;      // Walk flag

const Point SCREEN_CENTER = { WIDTH/2, HEIGHT/2 };


void Init() {
    // Window & buffer
    S_Init("Engine", WIDTH*2, HEIGHT*2);
    S_GrabMouse(1);
    buffer = B_CreateBuffer(WIDTH, HEIGHT);

    // Player
    player.position = (Vector){ 25, 25 };
    player.forward = G_Normalize((Vector){ 1, 1 });

    // Map
    map = M_Load("level.map");
    it = M_GetIterator(map);

    // Textures
    flor = S_LoadTexture("floor.bmp");
    wall = S_LoadTexture("wall.bmp");
    ceiling = S_LoadTexture("ceil.bmp");
}



void Quit() {
    S_Quit();
    M_DeleteIterator(it);
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

    D_DrawSegment(buffer, s1, WHITE);
    D_DrawSegment(buffer, s2, WHITE);
}



void DrawMap() {
    // TODO
}


void DrawCol(int x, int height, double distance, Wall *w, double offset) {
    int texture_col = MOD((int)offset, wall->width);

    for (int i = 0; i < height; i++) {
        int ii = WALLHEIGHT * i / height;

        int y = (buffer->height - height)/2 + i;

        if (y < 0 || y >= HEIGHT) continue;

        Color c = wall->pixels[ii * wall->width + texture_col];
        if (distance > FAR) {
            c = C_ScaleColor(c, FAR / distance);
        }

        D_DrawPixel(buffer, x, y, c);
    }
}


// Raycasting
void DrawPOV() {
    for (int x = 0; x < WIDTH; x++) {
        double ray_angle = atan2(x - (WIDTH / 2), VIEW);
        double ray_cos = cos(ray_angle);
        double viewcos = VIEW / ray_cos;  // Distance to the viewplane along the ray
        double nearcos = NEAR / ray_cos;  // Distance to the nearclipplane along the ray

        Ray ray = {
            .start = player.position,
            .dir = G_Rotate(player.forward, ray_angle)
        };

        double last_distance = DBL_MAX; // "Z Buffering"
        int last_height = 0;

        // Walls
        M_GoBeforeFirst(it);
        for (Wall *w = M_GetNext(it); w; w = M_GetNext(it)) {
            Vector hit;
            if (G_SegmentRayIntersection(w->seg, ray, &hit)) {
                double distance = G_Distance(hit, player.position);

                if (distance < nearcos) continue;
                if (distance > last_distance) continue;

                int col_height = viewcos * WALLHEIGHT / distance;
                DrawCol(x, col_height, distance, w, G_Distance(w->seg.start, hit));

                last_height = col_height;
                last_distance = distance;
            }
        }

        // Floor casting
        for (int h = 1 + (HEIGHT - last_height) / 2; h > 0; h--) {
            double texel_distance = (POVHEIGHT * viewcos) / ((HEIGHT / 2) - h);
            Point texel_world_pos = G_Sum(player.position, G_Scale(texel_distance, ray.dir));

            int texel_x = MOD((int)texel_world_pos.x, flor->width);
            int texel_y = MOD((int)texel_world_pos.y, flor->height);

            // Floor
            Color c = flor->pixels[texel_y * flor->width + texel_x];
            if (texel_distance > FAR) {
                c = C_ScaleColor(c, FAR / texel_distance);
            }
            D_DrawPixel(buffer, x, HEIGHT - h, c);

            // Ceiling
            c = ceiling->pixels[texel_y * flor->width + texel_x];
            if (texel_distance > FAR) {
                c = C_ScaleColor(c, FAR / texel_distance);
            }
            D_DrawPixel(buffer, x, h - 1, c);
        }
    }
}



void Draw() {
    DrawPOV();

    if (mapf) {
        DrawMap();
        DrawPlayer();
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
                G_Sum(
                    G_Scale(player.fwd, player.forward),
                    G_Scale(player.strafe, side)
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
