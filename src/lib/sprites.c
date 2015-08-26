#include <stdlib.h>

#include "buffer.h"
#include "load_image.h"
#include "sprites.h"


SpriteSheet SS_LoadSpriteSheet(const char *path, int rows, int cols) {
    Buffer *b = S_LoadImage(path);

    SpriteSheet ss = {
        .rows = rows,
        .cols = cols,
        .width =  b->width / cols,
        .height = b->height / rows,
        .sprites = malloc(sizeof(Buffer *) * rows * cols)
    };

    for (int j = 0; j < rows; j++) {
        for (int i = 0; i < cols; i++) {
            Buffer *sprite = B_GetSubBuffer(
                    b, i * ss.width, j * ss.height, ss.width, ss.height
                    );

            ss.sprites[j * cols + i] = sprite;
        }
    }

    B_DeleteBuffer(b);

    return ss;
}


Buffer *SS_GetSprite(SpriteSheet ss, int x, int y) {
    return ss.sprites[y * ss.cols + x];
}


void SS_DeleteSpriteSheet(SpriteSheet ss) {
    for (int i = 0; i < ss.rows * ss.cols; i++) {
        free(ss.sprites[i]);
    }
}
