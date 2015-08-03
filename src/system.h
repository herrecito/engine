//------------------------------------------------------------------------------
// Functions to abstract dependencies
//------------------------------------------------------------------------------

#ifndef _SYSTEM_
#define _SYSTEM_

#include <stdint.h>

#include "buffer.h"
#include "geometry.h"


//------------------------------------------------------------------------------
// Init / Shutdown
//------------------------------------------------------------------------------

// Initializes SDL. Creates a resizable window and handles resize events.
void S_Init(const char *title, int width, int height);

// Just calls SDL_Quit.
void S_Quit();



//------------------------------------------------------------------------------
// Video
//------------------------------------------------------------------------------

// Toggle fullscreen.
void S_ToggleFullcreen();

// Update the screen with the contents of buf.
void S_Blit(Buffer *buf);



//------------------------------------------------------------------------------
// Input
//------------------------------------------------------------------------------

// Returns a vector with the position of the mouse in buffer coordinates.
Vector S_GetMousePos(Buffer *buf);

// Set to 1 to keep the mouse inside the window.
void S_GrabMouse(int flag);

// Hack to work around a bug in SDL Relative Mouse Mode. Call in the main loop
// to keep the mouse inside the screen by warping it to the center.
//
// Doesn't do anything if the mouse isn't grabbed.
void S_MouseFix();



//------------------------------------------------------------------------------
// Resources
//------------------------------------------------------------------------------

typedef Buffer Texture;

// Load the texture given by path.
Texture *S_LoadTexture(const char *path);


#endif