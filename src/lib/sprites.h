//------------------------------------------------------------------------------
// Sprite sheets
//------------------------------------------------------------------------------
#ifndef _SPRITES_
#define _SPRITES_

#include "buffer.h"

typedef struct SpriteSheet {
    Buffer **sprites;
    int rows, cols;
    int width, height;
} SpriteSheet;


SpriteSheet SS_LoadSpriteSheet(const char *path, int rows, int cols);
Buffer *SS_GetSprite(SpriteSheet ss, int x, int y);
void SS_DeleteSpriteSheet(SpriteSheet ss);

#endif
