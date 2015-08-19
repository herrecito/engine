#include <stdlib.h>

#include "buffer.h"
#include "sprites.h"


Buffer *SS_GetSprite(SpriteSheet ss, int x, int y) {
    return ss.sprites[y * ss.cols + x];
}
