//------------------------------------------------------------------------------
// Spritesheets
//------------------------------------------------------------------------------
#ifndef _SPRITES_
#define _SPRITES_

#include "buffer.h"

typedef struct Sprites {
    Buffer **sprites;
    int rows, cols;
} Sprites;

#endif
