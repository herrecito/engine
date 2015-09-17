#ifndef _INPUT_
#define _INPUT_

#include <stdint.h>

#include <GLFW/glfw3.h>

#include "lib/geometry.h"

// Keys:    GLFW_KEY_*
// Actions: GLFW_{PRESS, RELEASE, REPEAT}
// Mods:    GLFW_MOD_{SHIFT, CONTROL, ALT, SUPER}
typedef struct KeyEvent {
    int key, action, mods;
} KeyEvent;


// Associates a KeyEvent with an action
typedef struct KeyBind {
    int key;
    void (*action)(KeyEvent kev);
} KeyBind;


// List of KeyBinds
typedef struct KeyMap {
    int numbinds;
    KeyBind *binds;
} KeyMap;


void PostEvent(KeyEvent kev);
uint32_t ProcessInput();

void KeyCB(GLFWwindow* window, int key, int scancode, int action, int mods);

#endif
