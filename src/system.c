#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL.h>
#include <SDL_image.h>

#include "geometry.h"
#include "buffer.h"
#include "system.h"
#include "dbg.h"

static const char *vertexSource = "#version 330 core\n"
"\n"
"in vec2 position;\n"
"in vec2 texcoord;\n"
"\n"
"out vec3 Color;\n"
"out vec2 Texcoord;\n"
"\n"
"void main() {\n"
"    Texcoord = texcoord;\n"
"    gl_Position = vec4(position, 0.0, 1.0);\n"
"}";

static const char *fragmentSource = "#version 330 core\n"
"\n"
"in vec2 Texcoord;\n"
"\n"
"out vec4 outColor;\n"
"\n"
"uniform sampler2D tex;\n"
"\n"
"void main() {\n"
"    outColor = texture(tex, Texcoord);\n"
"}";

static SDL_Window *window;
static SDL_GLContext glcontext;

// Flags
static int resizef;


void S_Fullscreen(int flag) {
    if (flag) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window, 0);
    }
}


void S_GrabMouse(int flag) {
    SDL_SetRelativeMouseMode(flag);
}


int HandleResize(void *userdata, SDL_Event *ev) {
    if (ev->type == SDL_WINDOWEVENT) {
        if (ev->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
            resizef = 1;
        }
    }

    return 0;  // Ignored
}


void S_Init(const char *title, int width, int height) {
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);

    // Core-Profile OpenGL 3.3
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,
            SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

    // Create window and OpenGL Context
    window = SDL_CreateWindow(
            title,
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            width, height,
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL
            );

    glcontext = SDL_GL_CreateContext(window);

    // Handle resize events
    SDL_AddEventWatch(HandleResize, NULL);

    // Disable vsync
    SDL_GL_SetSwapInterval(0);

    // Init GLEW
    glewExperimental = GL_TRUE;
    glewInit();

    // Quad vertices ...
    float vertices[] = {
        // Position   Texcoords
        -1,  1,   0, 0,  // Top-left
        1,  1,   1, 0,  // Top-right
        1, -1,   1, 1,  // Bottom-right
        -1, -1,   0, 1,  // Bottom-left
    };

    // ... elements ...
    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0,
    };

    // ... and texture
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, NULL);

    // Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Vertex Buffer Object
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Element Buffer Object
    GLuint ebo;
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
            GL_STATIC_DRAW);

    // Shaders
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Vertex attributes
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(float), 0);

    GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
    glEnableVertexAttribArray(texAttrib);
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE,
            4 * sizeof(float), (void *)(2 * sizeof(float)));

    // Clear with black
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}


void S_Quit() {
    SDL_GL_DeleteContext(glcontext);
    IMG_Quit();
    SDL_Quit();
}


uint32_t S_Blit(Buffer *buf) {
    uint32_t start = S_GetTime();

    if (resizef) {
        resizef = 0;

        int winwidth, winheight;
        SDL_GetWindowSize(window, &winwidth, &winheight);

        glViewport(0, 0, winwidth, winheight);
    }

    glTexSubImage2D(
            GL_TEXTURE_2D,
            0, 0,
            0,
            buf->width, buf->height,
            GL_RGBA, GL_UNSIGNED_BYTE,
            buf->pixels
            );

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    SDL_GL_SwapWindow(window);

    return S_GetTime() - start;
}


Vector S_GetMousePos(Buffer *buf) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);

    int winwidth, winheight;
    SDL_GetWindowSize(window, &winwidth, &winheight);

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


uint32_t S_GetTime() {
    return SDL_GetTicks();
}


void S_Sleep(uint32_t ms) {
    SDL_Delay(ms);
}
