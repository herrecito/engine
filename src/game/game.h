#ifndef _GAME_
#define _GAME_

#include <GLFW/glfw3.h>

#include "lib/buffer.h"
#include "lib/collision.h"
#include "lib/map.h"

#include "input.h"


// Player command
typedef struct Command {
    Vector velocity;
} Command;


#define MAXEVENTS 64

// Contains all the game global state
typedef struct Game {
    GLFWwindow *window;
    Buffer *buffer;

    Map *map;       // Current Map
    Mobile player;  // Player

    // --- Input ---
    KeyMap *keymap; // Current KeyMap

    int forward;
    int backward;
    int strafe_left;
    int strafe_right;

    int turn_left;
    int turn_right;
} Game;


extern Game game;

void InitGame();
void EndGame();
Scene GetScene();
Command BuildCommand();
void RunCommand(Command cmd);

#endif
