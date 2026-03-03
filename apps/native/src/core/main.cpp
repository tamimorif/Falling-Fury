#include "core/Game.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

namespace {
void emscriptenLoop(void* arg) {
    auto* game = static_cast<Game*>(arg);
    if (game->running() && !game->getEndGame()) {
        game->update();
        game->render();
        return;
    }

    emscripten_cancel_main_loop();
    delete game;
}
}  // namespace
#endif

int main() {
#ifdef __EMSCRIPTEN__
    auto* game = new Game();
    emscripten_set_main_loop_arg(emscriptenLoop, game, 0, 1);
#else
    // Init Game engine
    Game game;

    // Game Loop
    while (game.running() && !game.getEndGame()) {
        game.update();
        // render
        game.render();
    }
#endif

    return 0;
}
