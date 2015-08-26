#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_PSD
#define STBI_NO_TGA
#include "stb_image.h"

#include "buffer.h"


Buffer *S_LoadImage(const char *path) {
    int width, height;
    unsigned char *data = stbi_load(path, &width, &height, NULL, 4);

    Buffer *b = B_CreateBuffer(width, height);

    if (data != NULL) {
        // TODO create function to not have to free pixels
        free(b->pixels);
        b->pixels = (uint32_t *)data;
    } else {
        fprintf(stderr, "S_LoadImage(): Error loading: %s\n", path);
    }

    return b;
}
