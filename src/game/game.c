#include <GLFW/glfw3.h>

#include "lib/map.h"
#include "lib/system.h"

#include "actions.h"
#include "game.h"

Game game;

#define WIDTH  640
#define HEIGHT 400

void InitGame() {
    game.window = S_InitWindow(WIDTH, HEIGHT);
    glfwSetKeyCallback(game.window, KeyCB);
    game.buffer = B_CreateBuffer(WIDTH, HEIGHT);

    game.player = (Mobile){
        .pos = (Vector){ 0, 0 },
        .vel = (Vector){ 0, 0 },
        .forward = (Vector){ 1, 0 }
    };

    game.map = M_Load("level.map");
    game.keymap = GetDefaultKeyMap();
}


void EndGame() {
    glfwTerminate();
}


Scene GetScene() {
    Scene scene = {
        .pov = &game.player,
        .map = game.map
    };

    return scene;
}
