#include <SDL.h>
#include <SDL_image.h>

#include "geometry.h"
#include "buffer.h"
#include "system.h"
#include "dbg.h"


static SDL_Window *win;
static SDL_Surface *winsurf;

static Buffer *lastbuf;
static SDL_Surface *surf;

// Flags
static int fullscreenf;
static int grabmousef;


int HandleResize(void *userdata, SDL_Event *ev) {
    if (ev->type == SDL_WINDOWEVENT) {
        if (ev->window.event == SDL_WINDOWEVENT_RESIZED) {
            winsurf = SDL_GetWindowSurface(win);
        }
    }

    return 0;  // Ignored
}



void S_Fullscreen(int flag) {
    if (flag) {
        SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(win, 0);
    }

    fullscreenf = flag;
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
    SDL_AddEventWatch(HandleResize, NULL);
}


void S_Quit() {
    IMG_Quit();
    SDL_Quit();
}


void S_Blit(Buffer *buf) {
    if (buf != lastbuf) {
        lastbuf = buf;
        SDL_FreeSurface(surf);
        surf = SDL_CreateRGBSurfaceFrom(
                buf->pixels,                                    // Pixels
                buf->width, buf->height, 32, 4 * buf->width,    // Width, Height, Depth, Pitch
                0xFF0000, 0x00FF00, 0x0000FF, 0                 // RGBA Masks
                );
    }

    SDL_BlitScaled(surf, NULL, winsurf, NULL);
    SDL_UpdateWindowSurface(win);
}


void S_MouseFix() {
    int winwidth, winheight;
    SDL_GetWindowSize(win, &winwidth, &winheight);

    if (grabmousef) {
        SDL_WarpMouseInWindow(win, winwidth / 2, winheight / 2);
    }
}



Vector S_GetMousePos(Buffer *buf) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    int winwidth, winheight;
    SDL_GetWindowSize(win, &winwidth, &winheight);

    return (Vector){ mx * buf->width / winwidth, my * buf->height / winheight };
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


Buffer *S_LoadImage(const char *path) {
    SDL_Surface *tex_surf = IMG_Load(path);
    check(tex_surf,
            "Error loading texture. IMG_GetError(): %s\n", IMG_GetError());

    Buffer *t = B_CreateBuffer(tex_surf->w, tex_surf->h);

    for (int y = 0; y < tex_surf->h; y++) {
        for (int x = 0; x < tex_surf->w; x++) {
            uint32_t pixel = GetPixel(tex_surf, x, y);

            uint8_t red, green, blue;
            SDL_GetRGB(pixel, tex_surf->format, &red, &green, &blue);

            t->pixels[y * t->width + x] = BUILDRGB(red, green, blue);
        }
    }

    SDL_FreeSurface(tex_surf);

    return t;
}
