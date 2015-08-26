#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "game.h"
#include "input.h"


static void Action_Quit(KeyEvent kev) {
    glfwSetWindowShouldClose(game.window, 1);
}


static void Action_Forward(KeyEvent kev) {
    if (kev.action == GLFW_PRESS) {
        game.forward = 1;
    } else if (kev.action == GLFW_RELEASE) {
        game.forward = 0;
    }
}


static void Action_Backward(KeyEvent kev) {
    if (kev.action == GLFW_PRESS) {
        game.forward = -1;
    } else if (kev.action == GLFW_RELEASE) {
        game.forward = 0;
    }
}


KeyMap *GetDefaultKeyMap() {
    KeyMap *keymap = malloc(sizeof(KeyMap));
    keymap->numbinds = 3;

    keymap->binds = malloc(sizeof(KeyBind) * keymap->numbinds);
    keymap->binds[0] = (KeyBind){ GLFW_KEY_Q, Action_Quit     };
    keymap->binds[1] = (KeyBind){ GLFW_KEY_W, Action_Forward  };
    keymap->binds[2] = (KeyBind){ GLFW_KEY_S, Action_Backward };

    return keymap;
}
