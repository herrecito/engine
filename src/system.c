#include <SDL.h>
#include <SDL_image.h>

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
    IMG_Init(IMG_INIT_PNG);

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
    IMG_Quit();
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

// http://sdl.beuc.net/sdl.wiki/Pixel_Access
uint32_t GetPixel(SDL_Surface *surface, int x, int y) {
    int bpp = surface->format->BytesPerPixel;
    // Here p is the address to the pixel we want to retrieve.
    uint8_t *p = (uint8_t *)surface->pixels + y * surface->pitch + x * bpp;

    switch (bpp) {
        case 1:
            return *p;
            break;

        case 2:
            return *(uint16_t *)p;
            break;

        case 3:
            if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
                return p[0] << 16 | p[1] << 8 | p[2];
            } else {
                return p[0] | p[1] << 8 | p[2] << 16;
            }
            break;

        case 4:
            return *(uint32_t *)p;
            break;

        default:
            return 0;  // shouldn't happen, but avoids warnings.
    }
}


Buffer *S_LoadTexture(const char *path) {
    SDL_Surface *tex_surf = IMG_Load(path);
    check(tex_surf,
            "Error loading texture. IMG_GetError(): %s\n", IMG_GetError());

    Buffer *t = B_CreateBuffer(tex_surf->w, tex_surf->h);

    for (int x = 0; x < tex_surf->w; x++) {
        for (int y = 0; y < tex_surf->h; y++) {
            uint32_t pixel = GetPixel(tex_surf, x, y);

            uint8_t red, green, blue;
            SDL_GetRGB(pixel, tex_surf->format, &red, &green, &blue);

            t->pixels[y * t->width + x] = BUILDRGB(red, green, blue);
        }
    }

    SDL_FreeSurface(tex_surf);

    return t;
}
