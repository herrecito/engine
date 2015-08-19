#include <stdlib.h>

#include "buffer.h"
#include "sprites.h"


Buffer *SS_GetSprite(SpriteSheet ss, int x, int y) {
    return ss.sprites[y * ss.cols + x];
}


void SS_DeleteSpriteSheet(SpriteSheet ss) {
    for (int i = 0; i < ss.rows * ss.cols; i++) {
        free(ss.sprites[i]);
    }
}
