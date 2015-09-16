#ifndef _GAME_
#define _GAME_

#include <GLFW/glfw3.h>

#include "lib/buffer.h"
#include "lib/collision.h"
#include "lib/map.h"

#include "input.h"


#define MAXEVENTS 64

typedef struct Game {
    // main.c
    GLFWwindow *window;
    Buffer *buffer;

    Map *map;
    Mobile player;


    // --- Input ---
    KeyMap *keymap; // Current KeyMap

    int forward;
    int side;
    int turn;
} Game;


extern Game game;

void InitGame();
void EndGame();
Scene GetScene();

#endif
