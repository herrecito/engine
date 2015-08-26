#include <stdlib.h>

#include "lib/collision.h"
#include "lib/defs.h"
#include "lib/geometry.h"
#include "lib/system.h"

#include "game.h"
#include "input.h"


void PostEvent(KeyEvent kev) {
    game.events[game.eventhead] = kev;
    game.eventhead = (game.eventhead + 1) % MAXEVENTS;
}


Command BuildCommand() {
    Command cmd;
    cmd.velocity = G_Scale(game.forward, game.player.forward);
    return cmd;
}


void RunCommand(Command cmd) {
    game.player.vel = cmd.velocity;
    game.player = Co_Move(game.map, game.player);
}


uint32_t ProcessEvents() {
    uint32_t start = S_GetTime();

    for ( ; game.eventtail != game.eventhead;
            game.eventtail = (game.eventtail + 1) % MAXEVENTS)
    {
        KeyEvent kev = game.events[game.eventtail];

        // Find KeyBind that responds to this KeyEvent (if any)
        // and run the associated action.
        for (int i = 0; i < game.keymap->numbinds; i++) {
            KeyBind *bind = &game.keymap->binds[i];

            if (bind->key == kev.key) {
                bind->action(kev);
                break;
            }
        }
    }

    Command cmd = BuildCommand();
    RunCommand(cmd);

    return S_GetTime() - start;
}


void KeyCB(GLFWwindow* window, int key, int scancode, int action, int mods) {
    KeyEvent kev = { key, action, mods };

    PostEvent(kev);
}
