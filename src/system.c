#include <SDL.h>

#include "geometry.h"
#include "buffer.h"
#include "system.h"
#include "dbg.h"


static SDL_Window *win;
static SDL_Surface *winsurf;
static int winwidth;
static int winheight;

// Flags
static int fullscreenf;
static int grabmousef;


int HandleResize(void *userdata, SDL_Event *ev) {
    if (ev->type == SDL_WINDOWEVENT) {
        switch (ev->window.event) {
            case SDL_WINDOWEVENT_SIZE_CHANGED:
                winsurf = SDL_GetWindowSurface(win);
                SDL_GetWindowSize(win, &winwidth, &winheight);
                break;
        }
    }

    return 0;  // Ignored
}



void S_ToggleFullcreen() {
    if (fullscreenf) {
        SDL_SetWindowFullscreen(win, 0);
    } else {
        SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    fullscreenf = !fullscreenf;
}



void S_GrabMouse(int flag) {
    SDL_SetRelativeMouseMode(flag);
    grabmousef = flag;
}



void S_Init(const char *title, int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);

    // Window creation
    win = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_RESIZABLE
            );
    winsurf = SDL_GetWindowSurface(win);
    SDL_GetWindowSize(win, &winwidth, &winheight);
    SDL_AddEventWatch(HandleResize, NULL);
}


void S_Quit() {
    SDL_Quit();
}


void S_Blit(Buffer *buf) {
    double xscale = (double)buf->width / winwidth;
    double yscale = (double)buf->height / winheight;

    for (int x = 0; x < winwidth; x++) {
        for (int y = 0; y < winheight; y++) {
            int xx = x * xscale;
            int yy = y * yscale;
            int i = yy * buf->width + xx;

            ((uint32_t *)winsurf->pixels)[y * winwidth + x] = buf->pixels[i];
        }
    }

    SDL_UpdateWindowSurface(win);
}


void S_MouseFix() {
    if (grabmousef) {
        SDL_WarpMouseInWindow(win, winwidth / 2, winheight / 2);
    }
}



Vector S_GetMousePos(Buffer *buf) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    return (Vector){
        mx * buf->width / winwidth,
        my * buf->height / winheight
    };
}



Texture *S_LoadTexture(const char *path) {
    Texture *t = malloc(sizeof(Texture));

    SDL_Surface *tex_surf = SDL_LoadBMP(path);
    check(tex_surf,
            "Error loading texture. SDL_GetError(): %s\n", SDL_GetError());

    t->width = tex_surf->w;
    t->height = tex_surf->h;
    t->pixels = malloc(sizeof(t->pixels[0]) * t->width * t->height);

    for (int i = 0, j = 0; i < 3 * t->width * t->height; i += 3, j++) {
        uint32_t raw_pixel = *((uint32_t *)((uint8_t *)tex_surf->pixels + i));
        uint8_t red, green, blue;
        SDL_GetRGB(raw_pixel, tex_surf->format, &red, &green, &blue);
        t->pixels[j] = BUILDRGB(red, green, blue);
    }

    SDL_FreeSurface(tex_surf);

    return t;
}
