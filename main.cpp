#include "Game.h"
int main()
{
    // Init srand
    std::srand(static_cast<unsigned>(time(NULL)));

    // Init Game engine
    Game game;

    // Game Loop
    while (game.running() && !game.getEndGame() ghn)
    {
        // update
        game.update();
        // render
        game.render();
    }
}