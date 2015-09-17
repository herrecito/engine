#include <stdlib.h>

#include <GLFW/glfw3.h>

#include "lib/collision.h"
#include "lib/defs.h"
#include "lib/geometry.h"
#include "lib/system.h"

#include "game.h"
#include "input.h"


void DispatchEvent(KeyEvent kev) {
    for (int i = 0; i < game.keymap->numbinds; i++) {
        KeyBind *bind = &game.keymap->binds[i];

        if (bind->key == kev.key) {
            bind->action(kev);
            break;
        }
    }
}


uint32_t ProcessInput() {
    uint32_t start = S_GetTime();

    glfwPollEvents();

    Command cmd = BuildCommand();
    RunCommand(cmd);

    return S_GetTime() - start;
}


void KeyCB(GLFWwindow *window, int key, int scancode, int action, int mods) {
    KeyEvent kev = { key, action, mods };

    DispatchEvent(kev);
}
