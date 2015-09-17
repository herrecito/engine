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
        game.backward = 1;
    } else if (kev.action == GLFW_RELEASE) {
        game.backward = 0;
    }
}


static void Action_Strafe_Left(KeyEvent kev) {
    if (kev.action == GLFW_PRESS) {
        game.strafe_left = 1;
    } else if (kev.action == GLFW_RELEASE) {
        game.strafe_left = 0;
    }
}


static void Action_Strafe_Right(KeyEvent kev) {
    if (kev.action == GLFW_PRESS) {
        game.strafe_right = 1;
    } else if (kev.action == GLFW_RELEASE) {
        game.strafe_right = 0;
    }
}


static void Action_Turn_Left(KeyEvent kev) {
    if (kev.action == GLFW_PRESS) {
        game.turn_left = 1;
    } else if (kev.action == GLFW_RELEASE) {
        game.turn_left = 0;
    }
}


static void Action_Turn_Right(KeyEvent kev) {
    if (kev.action == GLFW_PRESS) {
        game.turn_right = 1;
    } else if (kev.action == GLFW_RELEASE) {
        game.turn_right = 0;
    }
}


KeyMap *GetDefaultKeyMap() {
    KeyMap *keymap = malloc(sizeof(KeyMap));
    keymap->numbinds = 7;

    keymap->binds = malloc(sizeof(KeyBind) * keymap->numbinds);
    keymap->binds[0] = (KeyBind){ GLFW_KEY_Q, Action_Quit         };
    keymap->binds[1] = (KeyBind){ GLFW_KEY_W, Action_Forward      };
    keymap->binds[2] = (KeyBind){ GLFW_KEY_S, Action_Backward     };
    keymap->binds[3] = (KeyBind){ GLFW_KEY_A, Action_Strafe_Left  };
    keymap->binds[4] = (KeyBind){ GLFW_KEY_D, Action_Strafe_Right };
    keymap->binds[5] = (KeyBind){ GLFW_KEY_H, Action_Turn_Left    };
    keymap->binds[6] = (KeyBind){ GLFW_KEY_L, Action_Turn_Right   };

    return keymap;
}
